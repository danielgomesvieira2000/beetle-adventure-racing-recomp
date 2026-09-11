//
// RT64
//

#include "rt64_rdp.h"

#include "render/rt64_vi_renderer.h"

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <cassert>
#include <set>

#include "../include/rt64_extended_gbi.h"

#include "common/rt64_math.h"
#include "gbi/rt64_f3d.h"

#include "rt64_bar_hud.h"
#include "rt64_color_converter.h"
#include "rt64_interpreter.h"
#include "rt64_state.h"

#ifndef NDEBUG
//#   define ASSERT_LOAD_METHODS
//#   define LOG_FILLRECT_METHODS
//#   define LOG_TEXRECT_METHODS
//#   define LOG_TILE_METHODS
//#   define LOG_COLOR_DEPTH_IMAGE_METHODS
//#   define LOG_TEXTURE_IMAGE_METHODS
//#   define LOG_LOAD_METHODS
#endif

namespace RT64 {
    // RDP
    static uint8_t getCommandLength(uint32_t commandId) {
        if (commandId == (G_TEXRECT & 0x3F) || commandId == (G_TEXRECTFLIP & 0x3F)) {
            return 2;
        }
        else if (commandId >= G_RDPTRI_BASE && commandId <= (uint32_t)RDPTriangle::MaxValue) {
            // Determine the triangle type.
            bool shaded = (commandId & (uint32_t)RDPTriangle::Shaded) != 0;
            bool textured = (commandId & (uint32_t)RDPTriangle::Textured) != 0;
            bool z_buffered = (commandId & (uint32_t)RDPTriangle::Depth) != 0;

            // Determine the length of this triangle command based on the flags it has set.
            uint8_t commandLength = triangleBaseWords;

            if (shaded) {
                commandLength += triangleShadeWords;
            }

            if (textured) {
                commandLength += triangleTexWords;
            }

            if (z_buffered) {
                commandLength += triangleDepthWords;
            }

            return commandLength;
        }
        else {
            return 1;
        }
    }

    RDP::RDP(State *state) : state(state) {
        memset(TMEM, 0, sizeof(TMEM));
        crashed = false;
        crashReason = CrashReason::None;

        // Set up the command lengths array.
        for (uint32_t commandId = 0; commandId < commandWordLengths.size(); commandId++) {
            commandWordLengths[commandId] = getCommandLength(commandId);
        }
    }

    void RDP::setGBI() {
        gbi = state->ext.interpreter->gbiManager.getGBIForRDP();
    }

    void RDP::reset() {
        colorCombinerStackSize = 1;
        envColorStackSize = 1;
        primColorStackSize = 1;
        primDepthStackSize = 1;
        blendColorStackSize = 1;
        fogColorStackSize = 1;
        fillColorStackSize = 1;
        scissorStackSize = 1;
        colorCombinerStack[0] = { 0, 0 };
        otherMode = { 0, 0 };
        envColorStack[0] = {0.0f, 0.0f, 0.0f, 0.0f};
        primColorStack[0] = { 0.0f, 0.0f, 0.0f, 0.0f };
        primLODStack[0] = { 0.0f, 0.0f };
        primDepthStack[0] = { 0.0f, 0.0f };
        fogColorStack[0] = { 0.0f, 0.0f, 0.0f, 0.0f };
        fillColorStack[0] = 0;
        blendColorStack[0] = { 0.0f, 0.0f, 0.0f, 0.0f };
        keyCenter = { 0.0f, 0.0f, 0.0f };
        keyScale = { 0.0f, 0.0f, 0.0f };
        convertK[0] = 0;
        convertK[1] = 0;
        convertK[2] = 0;
        convertK[3] = 0;
        convertK[4] = 0;
        convertK[5] = 0;
        scissorRectStack[0] = { 0, 0, 8192, 8192 };
        scissorModeStack[0] = 0;
        pendingCommandCurrentBytes = 0;
        pendingCommandRemainingBytes = 0;

        clearExtended();
    }

    void RDP::crash(CrashReason reason) {
        assert(reason != CrashReason::None);
        crashReason = reason;
        crashed = true;
        assert(false && "Crash screen not implemented.");
    }
    
    void RDP::checkFramebufferPair() {
        if (colorImage.changed || depthImage.changed) {
            state->flush();
            state->submitFramebufferPair(colorImage.changed ? FramebufferPair::FlushReason::ColorImageChanged : FramebufferPair::FlushReason::DepthImageChanged);

            const int workloadCursor = state->ext.workloadQueue->writeCursor;
            Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
            workload.addFramebufferPair(colorImage.address, colorImage.fmt, colorImage.siz, colorImage.width, depthImage.address);
            colorImage.changed = false;
            depthImage.changed = false;

            state->updateDrawStatusAttribute(DrawAttribute::FramebufferPair);
        }
    }
    
    void RDP::checkFramebufferOverlap(uint32_t tmemStart, uint32_t tmemWords, uint32_t tmemMask, uint32_t addressStart, uint32_t addressEnd, uint32_t tileWidth, uint32_t tileHeight, bool RGBA32, bool makeTileCopy) {
        auto &fbManager = state->framebufferManager;
        Framebuffer *fb = fbManager.findMostRecentContaining(addressStart, addressEnd);
        if (fb != nullptr) {
            const bool gpuCopiesEnabled = state->ext.emulatorConfig->framebuffer.copyWithGPU;
            if (!gpuCopiesEnabled) {
                return;
            }

            FramebufferTile fbTile;
            bool couldMakeTile = false;
            if (makeTileCopy) {
                // Find the best possible fitting GPU copy to store in this TMEM region.
                couldMakeTile = fbManager.makeFramebufferTile(fb, addressStart, addressEnd, tileWidth, tileHeight, fbTile, RGBA32);
            }

            // Always tags regions in TMEM, regardless of whether it's possible to make a copy or not.
            uint32_t fbEnd = fb->addressStart + fb->imageRowBytes(fb->width) * fb->maxHeight;
            bool syncRequired = (fb->addressStart < addressEnd) && (fbEnd > addressStart);
            fbManager.insertRegionsTMEM(fb->addressStart, tmemStart, std::min(tmemWords, uint32_t(RDP_TMEM_WORDS)), tmemMask, RGBA32, syncRequired, couldMakeTile ? &regionIterators : nullptr);
            
            if (couldMakeTile) {
                // Update the timestamp so the cache is discarded if they're not a match.
                fb->tileCopyCache.update(fbManager.getUsedTimestamp(), fb->lastWriteTimestamp);

                uint64_t newTileId;
                uint64_t tileCopyHash = fbTile.hash();
                auto it = fb->tileCopyCache.tileCopies.find(tileCopyHash);
                if (it != fb->tileCopyCache.tileCopies.end()) {
                    newTileId = it->second;
                }
                else {
                    // Make a new tile copy resource.
                    const uint32_t newTileWidth = fbTile.right - fbTile.left;
                    const uint32_t newTileHeight = fbTile.bottom - fbTile.top;
                    newTileId = fbManager.findTileCopyId(newTileWidth, newTileHeight);

                    // Queue the operation to make the tile copy.
                    FramebufferOperation fbOp = fbManager.makeTileCopyTMEM(newTileId, fbTile);
                    state->drawFbOperations.emplace_back(fbOp);

                    // Store the tile copy in the map.
                    fb->tileCopyCache.tileCopies[tileCopyHash] = newTileId;
                }

                // Store the FB tile and the copy ID in the relevant regions.
                for (FramebufferManager::RegionIterator regionIt : regionIterators) {
                    regionIt->fbTile = fbTile;
                    regionIt->tileCopyId = newTileId;
                }
            }
        }
    }
    
    void RDP::checkImageOverlap(uint32_t addressStart, uint32_t addressEnd) {
        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        FramebufferPair &fbPair = workload.fbPairs[workload.currentFramebufferPairIndex()];
        const FixedRect colorRect = fbPair.drawColorRect;
        if (colorRect.isEmpty()) {
            return;
        }
        
        const uint32_t imageWidth = fbPair.colorImage.width;
        const uint32_t colorRowStart = colorRect.top(false);
        const uint32_t colorRowEnd = colorRect.bottom(true);
        const uint32_t colorBpr = imageWidth << fbPair.colorImage.siz >> 1;
        const uint32_t colorStart = fbPair.colorImage.address + colorRowStart * colorBpr;
        const uint32_t colorEnd = colorStart + (colorRowEnd - colorRowStart) * colorBpr;
        bool overlapDetected = false;
        if ((addressStart < colorEnd) && (addressEnd > colorStart)) {
            colorImage.changed = true;
            overlapDetected = true;
        }
        else if (fbPair.depthWrite) {
            const FixedRect depthRect = fbPair.drawDepthRect;
            if (depthRect.isEmpty()) {
                return;
            }

            const uint32_t depthRowStart = depthRect.top(false);
            const uint32_t depthRowEnd = depthRect.bottom(true);
            const uint32_t depthBpr = imageWidth << G_IM_SIZ_16b >> 1;
            const uint32_t depthStart = fbPair.depthImage.address + depthRowStart * depthBpr;
            const uint32_t depthEnd = depthStart + (depthRowEnd - depthRowStart) * depthBpr;
            if ((addressStart < depthEnd) && (addressEnd > depthStart)) {
                depthImage.changed = true;
                overlapDetected = true;
            }
        }

        if (overlapDetected && (colorImage.changed || depthImage.changed)) {
            state->flush();
            state->submitFramebufferPair(colorImage.changed ? FramebufferPair::FlushReason::SamplingFromColorImage : FramebufferPair::FlushReason::SamplingFromDepthImage);
        }
    }
    
    int32_t RDP::movedFromOrigin(int32_t x, uint16_t ori) {
        if (ori < G_EX_ORIGIN_NONE) {
            return x + ((ori * colorImage.width * 4) / G_EX_ORIGIN_RIGHT);
        }
        else {
            return x;
        }
    };

    constexpr uint32_t ExtendedMask = 0x80000000U;

    uint32_t RDP::maskAddress(uint32_t address) {
        if (state->extended.extendRDRAM && ((address & ExtendedMask) == ExtendedMask)) {
            return address - ExtendedMask;
        }
        else {
            return address & RDP_ADDRESS_MASK;
        }
    }

    void RDP::setColorImage(uint8_t fmt, uint8_t siz, uint16_t width, uint32_t address) {
        // Make sure the new color image is actually different. Some games will set the color image
        // multiple times despite setting the exact same parameters.
        const uint32_t newAddress = maskAddress(address);
        if ((colorImage.fmt != fmt) ||
            (colorImage.siz != siz) ||
            (colorImage.width != width) ||
            (colorImage.address != newAddress))
        {
            colorImage.fmt = fmt;
            colorImage.siz = siz;
            colorImage.width = width;
            colorImage.address = newAddress;
            colorImage.changed = true;

#       ifdef LOG_COLOR_DEPTH_IMAGE_METHODS
            RT64_LOG_PRINTF("RDP::setColorImage(fmt %u, siz %u, width %u, address 0x%08X)", fmt, siz, width, address);
#       endif
        }
    }

    void RDP::setDepthImage(uint32_t address) {
        const uint32_t newAddress = maskAddress(address);
        if (depthImage.address != newAddress) {
            depthImage.address = newAddress;
            depthImage.changed = true;

#       ifdef LOG_COLOR_DEPTH_IMAGE_METHODS
            RT64_LOG_PRINTF("RDP::setDepthImage(address 0x%08X)", address);
#       endif
        }
    }

    void RDP::setTextureImage(uint8_t fmt, uint8_t siz, uint16_t width, uint32_t address) {
        texture.fmt = fmt;
        texture.siz = siz;
        texture.width = width;
        texture.address = maskAddress(address);
        state->updateDrawStatusAttribute(DrawAttribute::Texture);

#   ifdef LOG_TEXTURE_IMAGE_METHODS
        RT64_LOG_PRINTF("RDP::setTextureImage(fmt %u, siz %u, width %u, address 0x%08X)", fmt, siz, width, address);
#   endif
    }

    void RDP::setCombine(uint64_t combine) {
        interop::uint combineL = combine & 0xFFFFFFFFULL;
        interop::uint combineH = (combine >> 32ULL) & 0xFFFFFFFFULL;
        interop::ColorCombiner &colorCombiner = colorCombinerStack[colorCombinerStackSize - 1];
        if (colorCombiner.L != combineL || colorCombiner.H != combineH) {
            colorCombiner.L = combineL;
            colorCombiner.H = combineH;
            state->updateDrawStatusAttribute(DrawAttribute::Combine);
        }
    }

    void RDP::pushCombine() {
        if (colorCombinerStackSize < RDP_EXTENDED_STACK_SIZE) {
            colorCombinerStack[colorCombinerStackSize] = colorCombinerStack[colorCombinerStackSize - 1];
            colorCombinerStackSize++;
        }
    }

    void RDP::popCombine() {
        if (colorCombinerStackSize > 1) {
            colorCombinerStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::Combine);
        }
    }

    void RDP::setTile(uint8_t tile, uint8_t fmt, uint8_t siz, uint16_t line, uint16_t tmem, uint8_t palette, uint8_t cmt, uint8_t cms, uint8_t maskt, uint8_t masks, uint8_t shiftt, uint8_t shifts) {
#ifdef LOG_TILE_METHODS
        RT64_LOG_PRINTF("RDP::setTile(tile %u, fmt %u, siz %u, line %u, tmem %u, palette %u, cmt %u, cms %u, maskt %u, masks %u, shiftt %u, shifts %u)", tile, fmt, siz, line, tmem, palette, cmt, cms, maskt, masks, shiftt, shifts);
#endif

        assert(tile < RDP_TILES);

        auto &t = tiles[tile];
        t.fmt = fmt;
        t.siz = siz;
        t.line = line;
        t.tmem = tmem;
        t.palette = palette;
        t.cmt = cmt;
        t.cms = cms;
        t.masks = masks;
        t.maskt = maskt;
        t.shifts = shifts;
        t.shiftt = shiftt;
        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }

    void RDP::setTileSize(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt) {
#ifdef LOG_TILE_METHODS
        RT64_LOG_PRINTF("RDP::setTileSize(tile %u, uls %u, ult %u, lrs %u, lrt %u)", tile, uls, ult, lrs, lrt);
#endif

        assert(tile < RDP_TILES);
        auto &t = tiles[tile];
        t.uls = uls;
        t.ult = ult;
        t.lrs = lrs;
        t.lrt = lrt;
        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }

    void RDP::clearTileReplacementHash(uint8_t tile) {
        assert(tile < RDP_TILES);
        tileReplacementHashes[tile] = 0;
        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }

    void RDP::setTileReplacementHash(uint8_t tile, uint64_t replacementHash) {
        assert(tile < RDP_TILES);
        tileReplacementHashes[tile] = replacementHash;
        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }
    
    template<bool RGBA32 = false, bool TLUT = false>
    __forceinline void loadWord(uint8_t *TMEM, uint32_t tmemAddress, uint32_t tmemXorMask, const uint8_t *RDRAM, uint32_t textureAddress) {
        // Only sample the first two bytes in TLUT mode.
        uint32_t offsetMask;
        if constexpr (TLUT) {
            offsetMask = 0x1;
        }
        else {
            offsetMask = 0x7;
        }

        if constexpr (RGBA32) {
            // Split the lower and upper half of the word into the lower and upper half of TMEM.
            const uint32_t UpperTMEM = (RDP_TMEM_BYTES >> 1);
            TMEM[(tmemAddress + 0) ^ tmemXorMask] = RDRAM[(textureAddress + (0 & offsetMask)) ^ 3];
            TMEM[(tmemAddress + 1) ^ tmemXorMask] = RDRAM[(textureAddress + (1 & offsetMask)) ^ 3];
            TMEM[(tmemAddress + 2) ^ tmemXorMask] = RDRAM[(textureAddress + (4 & offsetMask)) ^ 3];
            TMEM[(tmemAddress + 3) ^ tmemXorMask] = RDRAM[(textureAddress + (5 & offsetMask)) ^ 3];
            TMEM[((tmemAddress + 0) ^ tmemXorMask) | UpperTMEM] = RDRAM[(textureAddress + (2 & offsetMask)) ^ 3];
            TMEM[((tmemAddress + 1) ^ tmemXorMask) | UpperTMEM] = RDRAM[(textureAddress + (3 & offsetMask)) ^ 3];
            TMEM[((tmemAddress + 2) ^ tmemXorMask) | UpperTMEM] = RDRAM[(textureAddress + (6 & offsetMask)) ^ 3];
            TMEM[((tmemAddress + 3) ^ tmemXorMask) | UpperTMEM] = RDRAM[(textureAddress + (7 & offsetMask)) ^ 3];
        }
        else {
            // Copy the entire word.
            for (uint32_t i = 0; i < 8; i++) {
                TMEM[(tmemAddress + i) ^ tmemXorMask] = RDRAM[(textureAddress + (i & offsetMask)) ^ 3];
            }
        }
    }

    template<bool RGBA32 = false, bool BLOCK = false, bool TLUT = false>
    __forceinline void loadToTMEMCommon(uint8_t *TMEM, const uint8_t *RDRAM, uint32_t textureStart, uint32_t textureStride, uint32_t tmemStart,
        uint32_t tmemStride, uint32_t wordsPerRow, uint32_t rowCount, uint32_t dxtIncrement = 0)
    {
        assert((!BLOCK || (rowCount == 1)) && "Load block must behave as if it only loads one row of data.");
        
        const uint32_t DXTSwap = 0x800;
        uint32_t textureAddress, tmemAddress, wordCount, tmemMask, tmemAdvance;
        if constexpr (RGBA32) {
            tmemMask = RDP_TMEM_MASK16;
            tmemAdvance = 0x4;
        }
        else {
            tmemMask = RDP_TMEM_MASK8;
            tmemAdvance = 0x8;
        }

        uint32_t textureAdvance;
        if constexpr (TLUT) {
            textureAdvance = 0x2;
        }
        else {
            textureAdvance = 0x8;
        }

        uint32_t tmemXorMask = 0x0;
        uint32_t dxtCounter = 0x0;
        auto loadWordStep = [&]() {
            if constexpr (BLOCK) {
                dxtCounter += dxtIncrement;
                while (dxtCounter >= DXTSwap) {
                    tmemAddress = (tmemAddress + tmemStride) & tmemMask;
                    dxtCounter -= DXTSwap;
                    tmemXorMask ^= 0x4;
                }
            }

            textureAddress += textureAdvance;
            tmemAddress = (tmemAddress + tmemAdvance) & tmemMask;
        };
        
        uint32_t textureAddressRow = textureStart;
        uint32_t tmemAddressRow = tmemStart & tmemMask;
        auto loadRowStep = [&]() {
            tmemAddressRow = (tmemAddressRow + tmemStride) & tmemMask;
            textureAddressRow += textureStride;
            tmemXorMask ^= 0x4;
        };

        // As an optimization for large texture loads that end up wrapping around in TMEM, we skip rows that have no effect in the final result.
        if ((tmemStride > 0) && ((wordsPerRow * tmemAdvance) <= tmemStride)) {
            int32_t rowsToSkip = rowCount - (tmemMask + tmemStride) / tmemStride;
            if (rowsToSkip > 0) {
                tmemAddressRow += (tmemAddressRow + tmemStride * rowsToSkip) & tmemMask;
                textureAddressRow += textureStride * rowsToSkip;
                tmemXorMask = (rowsToSkip & 0x1) << 2;
                rowCount -= rowsToSkip;
            }
        }

        while (rowCount > 0) {
            textureAddress = textureAddressRow;
            tmemAddress = tmemAddressRow;
            wordCount = wordsPerRow;
            while (wordCount > 0) {
                loadWord<RGBA32, TLUT>(TMEM, tmemAddress, tmemXorMask, RDRAM, textureAddress);
                loadWordStep();
                wordCount--;
            }

            loadRowStep();
            rowCount--;
        }
    }

    void RDP::loadTileOperation(const LoadTile &loadTile, const LoadTexture &loadTexture, bool deferred) {
        const uint32_t bytesOffset = (loadTile.uls >> 2) << loadTexture.siz >> 1;
        const uint32_t bytesPerRow = loadTexture.width << loadTexture.siz >> 1;
        const uint32_t textureStart = loadTexture.address + bytesOffset + bytesPerRow * (loadTile.ult >> 2);
        const uint32_t rowCount = 1 + ((loadTile.lrt >> 2) - (loadTile.ult >> 2));
        const uint32_t tileWidth = ((loadTile.lrs >> 2) - (loadTile.uls >> 2));
        const uint32_t wordsPerRow = (tileWidth >> (4 - loadTile.siz)) + 1;
        const uint32_t tmemStart = loadTile.tmem << 3;
        const uint32_t tmemStride = loadTile.line << 3;
        const uint32_t textureEnd = textureStart + (rowCount - 1) * bytesPerRow + (wordsPerRow << 3);
        const bool RGBA32 = (loadTile.siz == G_IM_SIZ_32b) && (loadTile.fmt == G_IM_FMT_RGBA);
        if (deferred) {
            checkImageOverlap(textureStart, textureEnd);

            // Discard any FB regions currently loaded into TMEM within the specified range
            const uint32_t wordShift = RGBA32 ? 2 : 3;
            const uint32_t tmemMask = RGBA32 ? RDP_TMEM_MASK128 : RDP_TMEM_MASK64;
            const uint32_t tmemBytes = (rowCount - 1) * tmemStride + (wordsPerRow << wordShift);
            const uint32_t tmemEnd = tmemStart + tmemBytes;
            state->framebufferManager.discardRegionsTMEM(tmemStart >> 3, tmemBytes >> 3, tmemMask);

            // Check for any GPU copies that can be made.
            const uint32_t lineShift = RGBA32 ? 1 : 0;
            const uint32_t lineWidth = loadTile.line << ((4 + lineShift) - loadTile.siz);
            checkFramebufferOverlap(tmemStart >> 3, tmemBytes >> 3, tmemMask, textureStart, textureEnd, lineWidth, rowCount, RGBA32, true);
        }
        else {
            // Load into TMEM.
            uint8_t *TMEM8 = reinterpret_cast<uint8_t *>(TMEM);
            const uint8_t *RDRAM = state->RDRAM;
            if (RGBA32) {
                loadToTMEMCommon<true>(TMEM8, RDRAM, textureStart, bytesPerRow, tmemStart, tmemStride, wordsPerRow, rowCount);
            }
            else {
                loadToTMEMCommon<false>(TMEM8, RDRAM, textureStart, bytesPerRow, tmemStart, tmemStride, wordsPerRow, rowCount);
            }
        }
    }

    void RDP::loadBlockOperation(const LoadTile &loadTile, const LoadTexture &loadTexture, bool deferred) {
        // Deduce loading parameters for block and the texture address.
        const uint32_t bytesOffset = loadTile.uls << loadTexture.siz >> 1;
        const uint32_t bytesPerRow = loadTexture.width << loadTexture.siz >> 1;
        const uint32_t textureStart = loadTexture.address + bytesOffset + bytesPerRow * loadTile.ult;
        const uint32_t wordCount = ((loadTile.lrs - loadTile.uls) >> (4 - loadTile.siz)) + 1;
        const uint32_t tmemStart = loadTile.tmem << 3;
        const uint32_t tmemStride = loadTile.line << 3;
        const uint32_t textureEnd = textureStart + (wordCount << 3);
        const bool RGBA32 = (loadTile.siz == G_IM_SIZ_32b) && (loadTile.fmt == G_IM_FMT_RGBA);
        if (deferred) {
            checkImageOverlap(textureStart, textureEnd);

            // Discard any FB regions currently loaded into TMEM within the specified range.
            const uint32_t wordShift = RGBA32 ? 2 : 3;
            const uint32_t tmemMask = RGBA32 ? RDP_TMEM_MASK128 : RDP_TMEM_MASK64;
            const uint32_t tmemBytes = (wordCount << wordShift);
            const uint32_t tmemEnd = tmemStart + tmemBytes;
            state->framebufferManager.discardRegionsTMEM(tmemStart >> 3, tmemBytes >> 3, tmemMask);

            // Check for any GPU copies that can be made.
            checkFramebufferOverlap(tmemStart >> 3, tmemBytes >> 3, tmemMask, textureStart, textureEnd, 0, 0, RGBA32, true);
        }
        else {
            // Load into TMEM.
            uint8_t *TMEM8 = reinterpret_cast<uint8_t *>(TMEM);
            const uint8_t *RDRAM = state->RDRAM;
            if (RGBA32) {
                loadToTMEMCommon<true, true>(TMEM8, RDRAM, textureStart, bytesPerRow, tmemStart, tmemStride, wordCount, 1, loadTile.lrt);
            }
            else {
                loadToTMEMCommon<false, true>(TMEM8, RDRAM, textureStart, bytesPerRow, tmemStart, tmemStride, wordCount, 1, loadTile.lrt);
            }
        }
    }

    void RDP::loadTLUTOperation(const LoadTile &loadTile, const LoadTexture &loadTexture, bool deferred) {
        // Deduce loading parameters for TLUT and the texture address.
        const uint32_t bytesOffset = (loadTile.uls >> 2) << loadTexture.siz >> 1;
        const uint32_t bytesPerRow = loadTexture.width << loadTexture.siz >> 1;
        const uint32_t textureStart = loadTexture.address + bytesOffset + bytesPerRow * (loadTile.ult >> 2);
        const uint32_t rowCount = 1 + ((loadTile.lrt >> 2) - (loadTile.ult >> 2));
        const uint32_t wordsPerRow = ((loadTile.lrs >> 2) - (loadTile.uls >> 2)) + 1;
        const uint32_t tmemStart = loadTile.tmem << 3;
        const uint32_t tmemStride = loadTile.line << 5;
        const bool RGBA32 = (loadTile.siz == G_IM_SIZ_32b) && (loadTile.fmt == G_IM_FMT_RGBA);
        if (deferred) {
            // Flush current framebuffer pair if any of the images being written to are loaded by this TLUT.
            const uint32_t textureEnd = textureStart + (rowCount - 1) * bytesPerRow + (wordsPerRow << 3);
            checkImageOverlap(textureStart, textureEnd);

            // Discard any FB regions currently loaded into TMEM within the specified range.
            const uint32_t wordShift = RGBA32 ? 0 : 1;
            const uint32_t tmemMask = RGBA32 ? RDP_TMEM_MASK128 : RDP_TMEM_MASK64;
            const uint32_t tmemBytes = (rowCount - 1) * tmemStride + (wordsPerRow << wordShift);
            const uint32_t tmemEnd = tmemStart + tmemBytes;
            state->framebufferManager.discardRegionsTMEM(tmemStart >> 3, tmemBytes >> 3, tmemMask);

            // Mark the TMEM regions with the loaded TLUT.
            const uint32_t lineShift = RGBA32 ? 1 : 0;
            const uint32_t tileWidth = loadTile.line << ((4 + lineShift) - loadTile.siz);
            checkFramebufferOverlap(tmemStart >> 3, tmemBytes >> 3, tmemMask, textureStart, textureEnd, 0, 0, RGBA32, false);
        }
        else {
            // Load into TMEM.
            uint8_t *TMEM8 = reinterpret_cast<uint8_t *>(TMEM);
            const uint8_t *RDRAM = state->RDRAM;
            if (RGBA32) {
                loadToTMEMCommon<true, false, true>(TMEM8, RDRAM, textureStart, bytesPerRow, tmemStart, tmemStride, wordsPerRow, rowCount);
            }
            else {
                loadToTMEMCommon<false, false, true>(TMEM8, RDRAM, textureStart, bytesPerRow, tmemStart, tmemStride, wordsPerRow, rowCount);
            }
        }
    }

    void RDP::loadTile(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt) {
#ifdef LOG_LOAD_METHODS
        RT64_LOG_PRINTF("RDP::loadTile(tile %u, uls %u, ult %u, lrs %u, lrt %u)", tile, uls, ult, lrs, lrt);
#endif
        assert(tile < RDP_TILES);
        auto &t = tiles[tile];
        t.uls = uls;
        t.ult = ult;
        t.lrs = lrs;
        t.lrt = lrt;

        // Ignored by the hardware.
        if (t.uls > t.lrs) {
            return;
        }

#   ifdef ASSERT_LOAD_METHODS
        assert((t.uls != t.lrs) || ((t.uls & 0x3) == 0) && "Unknown and possibly undefined hardware behavior.");
        assert((t.ult != t.lrt) || ((t.ult & 0x3) == 0) && "Unknown hardware behavior.");
        assert((t.lrt <= t.lrt) && "Unknown hardware behavior.");
        assert((t.siz == texture.siz) && "Different tile and texture sizes are not currently supported.");
        assert((texture.siz != G_IM_SIZ_4b) && "4-bit texture image is not currently supported.");
        assert((t.fmt != G_IM_FMT_YUV) && "YUV is not currently supported.");
        assert(((t.siz != G_IM_SIZ_32b) || (t.fmt == G_IM_FMT_RGBA)) && "Other 32-bit formats than RGBA32 are not currently supported.");
#   endif

        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        const bool warningsEnabled = state->ext.userConfig->developerMode;
        if (warningsEnabled) {
            const uint32_t loadIndex = uint32_t(workload.drawData.loadOperations.size());
            if (t.siz != texture.siz) {
                CommandWarning warning = CommandWarning::format("Load Operation #%u: RDP::loadTile called with texture image siz %u and "
                    "tile descriptor #%u with siz %u. Pixel size mismatch might not work correctly.", loadIndex, texture.siz, tile, t.siz);

                warning.indexType = CommandWarning::IndexType::LoadIndex;
                warning.load.index = loadIndex;
                workload.commandWarnings.emplace_back(warning);
            }
        }

        // Perform the first step of the deferred operation.
        loadTileOperation(t, texture, true);

        // Store the operation.
        LoadOperation operation;
        operation.type = LoadOperation::Type::Tile;
        operation.tile = t;
        operation.texture = texture;
        auto &opTile = operation.operationTile;
        opTile.tile = tile;
        opTile.uls = uls;
        opTile.ult = ult;
        opTile.lrs = lrs;
        opTile.lrt = lrt;
        workload.drawData.loadOperations.emplace_back(operation);

        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }

    bool RDP::loadTileCopyCheck(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt) {
        assert(tile < RDP_TILES);

        // Ignored by the hardware.
        if (uls > lrs) {
            return false;
        }

        const bool gpuCopiesEnabled = state->ext.emulatorConfig->framebuffer.copyWithGPU;
        if (!gpuCopiesEnabled) {
            return false;
        }

        // Current framebuffer pair must be flushed if we wish to do a tile copy with any of the bytes it uses.
        const auto &t = tiles[tile];
        const uint32_t bytesOffset = (uls >> 2) << texture.siz >> 1;
        const uint32_t bytesPerRow = texture.width << texture.siz >> 1;
        const uint32_t textureStart = texture.address + bytesOffset + bytesPerRow * (ult >> 2);
        const uint32_t rowCount = 1 + ((lrt >> 2) - (ult >> 2));
        const uint32_t tileWidth = ((lrs >> 2) - (uls >> 2));
        const uint32_t wordsPerRow = (tileWidth >> (4 - t.siz)) + 1;
        const uint32_t textureSize = (rowCount - 1) * bytesPerRow + (wordsPerRow << 3);
        const uint32_t textureEnd = textureStart + textureSize;
        checkImageOverlap(textureStart, textureEnd);

        Framebuffer *framebuffer = state->framebufferManager.findMostRecentContaining(textureStart, textureEnd);
        if (framebuffer != nullptr) {
            FramebufferTile fbTile;
            const bool RGBA32 = (t.siz == G_IM_SIZ_32b) && (t.fmt == G_IM_FMT_RGBA);
            const uint32_t lineShift = RGBA32 ? 1 : 0;
            const uint32_t lineWidth = t.line << ((4 + lineShift) - t.siz);
            return state->framebufferManager.makeFramebufferTile(framebuffer, textureStart, textureEnd, lineWidth, rowCount, fbTile, RGBA32);
        }
        else {
            return false;
        }
    }

    bool RDP::loadTileReplacementCheck(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt, uint8_t imageSiz, uint8_t imageFmt, uint16_t imageLoad, uint16_t imagePal, uint64_t &replacementHash) {
        assert(tile < RDP_TILES);

        // Ignored by the hardware.
        if (uls > lrs) {
            return false;
        }

        const auto &t = tiles[tile];
        const uint32_t bytesOffset = (uls >> 2) << texture.siz >> 1;
        const uint32_t bytesPerRow = texture.width << texture.siz >> 1;
        const uint32_t textureStart = texture.address + bytesOffset + bytesPerRow * (ult >> 2);
        const uint32_t rowCount = 1 + ((lrt >> 2) - (ult >> 2));
        const uint32_t tileWidth = ((lrs >> 2) - (uls >> 2));
        const uint32_t wordsPerRow = (tileWidth >> (4 - t.siz)) + 1;
        const uint32_t textureSize = (rowCount - 1) * bytesPerRow + (wordsPerRow << 3);

        // Hash the entire texture's memory along with the parameters that affect its format.
        XXH3_state_t xxh3;
        XXH3_64bits_reset(&xxh3);
        XXH3_64bits_update(&xxh3, &state->RDRAM[textureStart], textureSize);
        XXH3_64bits_update(&xxh3, &imageSiz, sizeof(imageSiz));
        XXH3_64bits_update(&xxh3, &imageFmt, sizeof(imageFmt));
        XXH3_64bits_update(&xxh3, &imageLoad, sizeof(imageLoad));
        XXH3_64bits_update(&xxh3, &imagePal, sizeof(imagePal));
        replacementHash = XXH3_64bits_digest(&xxh3);

        // Return whether a replacement exists for this hash or not.
        return state->ext.textureCache->hasReplacement(replacementHash);
    }

    void RDP::loadBlock(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t dxt) {
#ifdef LOG_LOAD_METHODS
        RT64_LOG_PRINTF("RDP::loadBlock(tile %u, uls %u, ult %u, lrs %u, dxt %u)", tile, uls, ult, lrs, dxt);
#endif
        assert(tile < RDP_TILES);
        auto &t = tiles[tile];
        t.uls = uls;
        t.ult = ult;
        t.lrs = lrs;
        t.lrt = dxt;

        // Ignored by the hardware.
        if ((t.uls > t.lrs) || (t.lrs >= 0x800)) {
            return;
        }
        
#   ifdef ASSERT_LOAD_METHODS
        assert((t.uls != t.lrs) || ((t.uls & 0x3) == 0) && "Unknown and possibly undefined hardware behavior.");
        assert((t.siz == texture.siz) && "Different tile and texture sizes are not currently supported.");
        assert((texture.siz != G_IM_SIZ_4b) && "4-bit texture image is not currently supported.");
        assert((t.fmt != G_IM_FMT_YUV) && "YUV is not currently supported.");
        assert(((t.siz != G_IM_SIZ_32b) || (t.fmt == G_IM_FMT_RGBA)) && "Other 32-bit formats than RGBA32 are not currently supported.");
#   endif

        // Check for warnings in developer mode.
        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        const bool warningsEnabled = state->ext.userConfig->developerMode;
        if (warningsEnabled) {
            const uint32_t loadIndex = uint32_t(workload.drawData.loadOperations.size());
            if (t.siz != texture.siz) {
                CommandWarning warning = CommandWarning::format("Load Operation #%u: RDP::loadBlock called with texture image siz %u and "
                    "tile descriptor #%u with siz %u. Pixel size mismatch might not work correctly.", loadIndex, texture.siz, tile, t.siz);

                warning.indexType = CommandWarning::IndexType::LoadIndex;
                warning.load.index = loadIndex;
                workload.commandWarnings.emplace_back(warning);
            }
        }

        // Perform the first step of the deferred operation.
        loadBlockOperation(t, texture, true);

        // Store the operation.
        LoadOperation operation;
        operation.type = LoadOperation::Type::Block;
        operation.tile = t;
        operation.texture = texture;
        auto &opBlock = operation.operationBlock;
        opBlock.tile = tile;
        opBlock.uls = uls;
        opBlock.ult = ult;
        opBlock.lrs = lrs;
        opBlock.dxt = dxt;
        workload.drawData.loadOperations.emplace_back(operation);

        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }

    void RDP::loadTLUT(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt) {
#ifdef LOG_LOAD_METHODS
        RT64_LOG_PRINTF("RDP::loadTLUT(tile %u, uls %u, ult %u, lrs %u, lrt %u)", tile, uls, ult, lrs, lrt);
#endif
        assert(tile < RDP_TILES);
        auto &t = tiles[tile];
        t.uls = uls;
        t.ult = ult;
        t.lrs = lrs;
        t.lrt = lrt;

#   ifdef ASSERT_LOAD_METHODS
        assert((texture.siz == G_IM_SIZ_16b) && "Non-16 bit textures are not currently supported.");
        assert((t.siz == G_IM_SIZ_4b) && "Non-4 bit tiles are not currently supported.");
#   endif
        
        // Check for warnings in developer mode.
        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        const bool warningsEnabled = state->ext.userConfig->developerMode;
        if (warningsEnabled) {
            const uint32_t loadIndex = uint32_t(workload.drawData.loadOperations.size());
            if (texture.siz != G_IM_SIZ_16b) {
                CommandWarning warning = CommandWarning::format("Load Operation #%u: RDP::loadTLUT called with incorrect texture image siz %u. "
                    "Loading TLUTs that don't use 16-bit formats might not work correctly.", loadIndex, texture.siz);

                warning.indexType = CommandWarning::IndexType::LoadIndex;
                warning.load.index = loadIndex;
                workload.commandWarnings.emplace_back(warning);
            }
            
            if (t.siz != G_IM_SIZ_4b) {
                CommandWarning warning = CommandWarning::format("Load Operation #%u: RDP::loadTLUT called with incorrect tile descriptor #%u with siz %u. "
                    "Loading TLUTs that don't use 4-bit formats as their tile size might not work correctly.", loadIndex, tile, t.siz);

                warning.indexType = CommandWarning::IndexType::LoadIndex;
                warning.load.index = loadIndex;
                workload.commandWarnings.emplace_back(warning);
            }
        }

        // Perform the first step of the deferred operation.
        loadTLUTOperation(t, texture, true);

        // Store the operation.
        LoadOperation operation;
        operation.type = LoadOperation::Type::TLUT;
        operation.tile = t;
        operation.texture = texture;
        auto &opTLUT = operation.operationTLUT;
        opTLUT.tile = tile;
        opTLUT.uls = uls;
        opTLUT.ult = ult;
        opTLUT.lrs = lrs;
        opTLUT.lrt = lrt;
        workload.drawData.loadOperations.emplace_back(operation);

        state->updateDrawStatusAttribute(DrawAttribute::Texture);
    }

    void RDP::setEnvColor(uint32_t color) {
        hlslpp::float4 &envColor = envColorStack[envColorStackSize - 1];
        envColor.x = ((color >> 24) & 0xFF) / 255.0f;
        envColor.y = ((color >> 16) & 0xFF) / 255.0f;
        envColor.z = ((color >> 8) & 0xFF) / 255.0f;
        envColor.w = ((color >> 0) & 0xFF) / 255.0f;
        state->updateDrawStatusAttribute(DrawAttribute::EnvColor);
    }

    void RDP::pushEnvColor() {
        if (envColorStackSize < RDP_EXTENDED_STACK_SIZE) {
            envColorStack[envColorStackSize] = envColorStack[envColorStackSize - 1];
            envColorStackSize++;
        }
    }

    void RDP::popEnvColor() {
        if (envColorStackSize > 1) {
            envColorStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::EnvColor);
        }
    }
    
    void RDP::setPrimColor(uint8_t lodFrac, uint8_t lodMin, uint32_t color) {
        hlslpp::float2 &primLOD = primLODStack[primColorStackSize - 1];
        primLOD.x = lodFrac / 256.0f;
        primLOD.y = lodMin / 32.0f;

        hlslpp::float4 &primColor = primColorStack[primColorStackSize - 1];
        primColor.x = ((color >> 24) & 0xFF) / 255.0f;
        primColor.y = ((color >> 16) & 0xFF) / 255.0f;
        primColor.z = ((color >> 8) & 0xFF) / 255.0f;
        primColor.w = ((color >> 0) & 0xFF) / 255.0f;
        state->updateDrawStatusAttribute(DrawAttribute::PrimColor);
    }

    void RDP::pushPrimColor() {
        if (primColorStackSize < RDP_EXTENDED_STACK_SIZE) {
            primColorStack[primColorStackSize] = primColorStack[primColorStackSize - 1];
            primLODStack[primColorStackSize] = primLODStack[primColorStackSize - 1];
            primColorStackSize++;
        }
    }

    void RDP::popPrimColor() {
        if (primColorStackSize > 1) {
            primColorStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::PrimColor);
        }
    }

    void RDP::setBlendColor(uint32_t color) {
        hlslpp::float4 &blendColor = blendColorStack[blendColorStackSize - 1];
        blendColor.x = ((color >> 24) & 0xFF) / 255.0f;
        blendColor.y = ((color >> 16) & 0xFF) / 255.0f;
        blendColor.z = ((color >> 8) & 0xFF) / 255.0f;
        blendColor.w = ((color >> 0) & 0xFF) / 255.0f;
        state->updateDrawStatusAttribute(DrawAttribute::BlendColor);
    }

    void RDP::pushBlendColor() {
        if (blendColorStackSize < RDP_EXTENDED_STACK_SIZE) {
            blendColorStack[blendColorStackSize] = blendColorStack[blendColorStackSize - 1];
            blendColorStackSize++;
        }
    }

    void RDP::popBlendColor() {
        if (blendColorStackSize > 1) {
            blendColorStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::BlendColor);
        }
    }

    void RDP::setFogColor(uint32_t color) {
        hlslpp::float4 &fogColor = fogColorStack[fogColorStackSize - 1];
        fogColor.x = ((color >> 24) & 0xFF) / 255.0f;
        fogColor.y = ((color >> 16) & 0xFF) / 255.0f;
        fogColor.z = ((color >> 8) & 0xFF) / 255.0f;
        fogColor.w = ((color >> 0) & 0xFF) / 255.0f;
        state->updateDrawStatusAttribute(DrawAttribute::FogColor);
    }

    void RDP::pushFogColor() {
        if (fogColorStackSize < RDP_EXTENDED_STACK_SIZE) {
            fogColorStack[fogColorStackSize] = fogColorStack[fogColorStackSize - 1];
            fogColorStackSize++;
        }
    }

    void RDP::popFogColor() {
        if (fogColorStackSize > 1) {
            fogColorStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::FogColor);
        }
    }

    void RDP::setFillColor(uint32_t color) {
        fillColorStack[fillColorStackSize - 1] = color;
        state->updateDrawStatusAttribute(DrawAttribute::FillColor);
    }

    void RDP::pushFillColor() {
        if (fillColorStackSize < RDP_EXTENDED_STACK_SIZE) {
            fillColorStack[fillColorStackSize] = fillColorStack[fillColorStackSize - 1];
            fillColorStackSize++;
        }
    }

    void RDP::popFillColor() {
        if (fillColorStackSize > 1) {
            fillColorStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::FillColor);
        }
    }

    void RDP::setOtherMode(uint32_t high, uint32_t low) {
        if (otherMode.H != high || otherMode.L != low) {
            otherMode.H = high;
            otherMode.L = low;
            state->updateDrawStatusAttribute(DrawAttribute::OtherMode);
        }
    }
    
    void RDP::setPrimDepth(uint16_t z, uint16_t dz) {
        const float Fixed15ToFloat = 1.0f / 32767.0f;
        const float Fixed16ToFloat = 1.0f / 65535.0f;
        hlslpp::float2 &primDepth = primDepthStack[primDepthStackSize - 1];
        primDepth.x = (z & 0x7FFFU) * Fixed15ToFloat;
        primDepth.y = (dz & 0xFFFFU) * Fixed16ToFloat;
        state->updateDrawStatusAttribute(DrawAttribute::PrimDepth);
    }

    void RDP::setScissor(uint8_t mode, int32_t ulx, int32_t uly, int32_t lrx, int32_t lry) {
        setScissor(mode, ulx, uly, lrx, lry, extended.global.scissor);
    }
    
    void RDP::setScissor(uint8_t mode, int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, const ExtendedAlignment &extAlignment) {
        FixedRect &scissorRect = scissorRectStack[scissorStackSize - 1];
        scissorRect.ulx = std::clamp(movedFromOrigin(ulx + extAlignment.leftOffset, extAlignment.leftOrigin), extAlignment.leftBound, extAlignment.rightBound);
        scissorRect.uly = std::clamp(uly + extAlignment.topOffset, extAlignment.topBound, extAlignment.bottomBound);
        scissorRect.lrx = std::clamp(movedFromOrigin(lrx + extAlignment.rightOffset, extAlignment.rightOrigin), extAlignment.leftBound, extAlignment.rightBound);
        scissorRect.lry = std::clamp(lry + extAlignment.bottomOffset, extAlignment.topBound, extAlignment.bottomBound);
        // BAR_DBG_SCISSOR=1 logs each DISTINCT scissor rectangle the game sets, in both raw 10.2
        // fixed point and pixels. BAR's uvgfxmgr emits these from uvGfxClipRect, so this shows the
        // rectangle the game itself is asking for -- which is how to tell a game-drawn inset (black
        // bars baked into the 320x240 image) apart from anything the renderer's window fit is doing.
        {
            static const bool scDbg = std::getenv("BAR_DBG_SCISSOR") != nullptr;
            if (scDbg) {
                static int32_t lastUlx = INT32_MIN, lastUly = 0, lastLrx = 0, lastLry = 0;
                if (ulx != lastUlx || uly != lastUly || lrx != lastLrx || lry != lastLry) {
                    lastUlx = ulx; lastUly = uly; lastLrx = lrx; lastLry = lry;
                    fprintf(stderr, "[scissor] raw=(%d,%d)-(%d,%d)  px=(%.1f,%.1f)-(%.1f,%.1f)  w=%.1f h=%.1f\n",
                        ulx, uly, lrx, lry, ulx / 4.0, uly / 4.0, lrx / 4.0, lry / 4.0,
                        (lrx - ulx) / 4.0, (lry - uly) / 4.0);
                    fflush(stderr);
                }
            }
        }
        // BAR: tell the present stage which region of the framebuffer this screen draws into.
        // Raw scissor coordinates are 10.2 fixed point, so convert to framebuffer pixels.
        VIRenderer::barReportGameScissor(ulx / 4.0f, uly / 4.0f, lrx / 4.0f, lry / 4.0f);

        scissorModeStack[scissorStackSize - 1] = mode;
        extended.scissorLeftOriginStack[scissorStackSize - 1] = extAlignment.leftOrigin;
        extended.scissorRightOriginStack[scissorStackSize - 1] = extAlignment.rightOrigin;
        state->updateDrawStatusAttribute(DrawAttribute::Scissor);
    }

    void RDP::pushScissor() {
        if (scissorStackSize < RDP_EXTENDED_STACK_SIZE) {
            scissorRectStack[scissorStackSize] = scissorRectStack[scissorStackSize - 1];
            scissorModeStack[scissorStackSize] = scissorModeStack[scissorStackSize - 1];
            extended.scissorLeftOriginStack[scissorStackSize] = extended.scissorLeftOriginStack[scissorStackSize - 1];
            extended.scissorRightOriginStack[scissorStackSize] = extended.scissorRightOriginStack[scissorStackSize - 1];
            scissorStackSize++;
        }
    }

    void RDP::popScissor() {
        if (scissorStackSize > 1) {
            scissorStackSize--;
            state->updateDrawStatusAttribute(DrawAttribute::Scissor);
        }
    }

    void RDP::setConvert(int32_t k0, int32_t k1, int32_t k2, int32_t k3, int32_t k4, int32_t k5) {
        convertK[0] = k0;
        convertK[1] = k1;
        convertK[2] = k2;
        convertK[3] = k3;
        convertK[4] = k4;
        convertK[5] = k5;
        state->updateDrawStatusAttribute(DrawAttribute::Convert);
    }

    void RDP::setKeyR(uint32_t cR, uint32_t sR, uint32_t wR) {
        // Width is ignored until its exact purpose is understood on the chroma keying process.
        keyCenter.x = cR / 255.0f;
        keyScale.x = sR / 255.0f;
        state->updateDrawStatusAttribute(DrawAttribute::Key);
    }

    void RDP::setKeyGB(uint32_t cG, uint32_t sG, uint32_t wG, uint32_t cB, uint32_t sB, uint32_t wB) {
        // Width is ignored until its exact purpose is understood on the chroma keying process.
        keyCenter.y = cG / 255.0f;
        keyCenter.z = cB / 255.0f;
        keyScale.y = sG / 255.0f;
        keyScale.z = sB / 255.0f;
        state->updateDrawStatusAttribute(DrawAttribute::Key);
    }

    void RDP::fillRect(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry) {
        fillRect(ulx, uly, lrx, lry, extended.global.rect);
    }

    void RDP::fillRect(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, const ExtendedAlignment &extAlignment) {
#   ifdef LOG_FILLRECT_METHODS
        RT64_LOG_PRINTF("RDP::fillRect(ulx %d, uly %d, lrx %d, lry %d)", ulx, uly, lrx, lry);
#   endif

        // Filter out incorrect rectangles.
        if ((lrx < ulx) || (lry < uly)) {
            return;
        }

        int32_t mode = (otherMode.H & (3U << G_MDSFT_CYCLETYPE));
        if ((mode == G_CYC_COPY) || (mode == G_CYC_FILL)) {
            lrx |= 3;
            lry |= 3;
        }

        drawRect(ulx, uly, lrx, lry, 0, 0, 0, 0, false, extAlignment);
    }

    void RDP::setRectAlign(const ExtendedAlignment &extAlignment) {
        extended.global.rect = extAlignment;
    }

    void RDP::setScissorAlign(const ExtendedAlignment &extAlignment) {
        extended.global.scissor = extAlignment;
    }

    void RDP::setRectAspect(uint8_t aspect) {
        extended.global.rectAspect = aspect;
    }

    void RDP::forceUpscale2D(bool force) {
        extended.drawExtendedFlags.forceUpscale2D = force;
        state->updateDrawStatusAttribute(DrawAttribute::ExtendedFlags);
    }
    
    void RDP::forceTrueBilerp(uint8_t mode) {
        extended.drawExtendedFlags.forceTrueBilerp = mode;
        state->updateDrawStatusAttribute(DrawAttribute::ExtendedFlags);
    }

    void RDP::forceScaleLOD(bool force) {
        extended.drawExtendedFlags.forceScaleLOD = force;
        state->updateDrawStatusAttribute(DrawAttribute::ExtendedFlags);
    }

    void RDP::clearExtended() {
        extended.scissorLeftOriginStack[0] = G_EX_ORIGIN_NONE;
        extended.scissorRightOriginStack[0] = G_EX_ORIGIN_NONE;
        extended.drawExtendedFlags = {};
        extended.global.rect = ExtendedAlignment();
        extended.global.scissor = ExtendedAlignment();
        extended.global.rectAspect = G_EX_ASPECT_AUTO;
    }
    
    void RDP::drawTris(uint32_t triCount, const float *pos, const float *tc, const float *col, uint8_t tile, uint8_t levels) {
        // Levels is expected to be above 0 elsewhere.
        levels += 1;

        // Check if the texture needs to be updated.
        DrawCall &drawCall = state->drawCall;
        if (!drawCall.textureOn || (drawCall.textureTile != tile) || (drawCall.textureLevels != levels)) {
            drawCall.textureOn = 1;
            drawCall.textureTile = tile;
            drawCall.textureLevels = levels;
            state->updateDrawStatusAttribute(DrawAttribute::Texture);
        }

        // Check if framebuffer pair must be changed.
        checkFramebufferPair();

        // Change projection to triangles.
        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        FramebufferPair &fbPair = workload.fbPairs[workload.currentFramebufferPairIndex()];
        if (!fbPair.inProjection(0, Projection::Type::Triangle)) {
            state->flush();
            fbPair.changeProjection(0, Projection::Type::Triangle);
        }

        bool flushedState = state->checkDrawState();

        // We only change these once the draw state has been checked.
        drawCall.minWorldMatrix = 0;
        drawCall.maxWorldMatrix = 0;

        if (flushedState) {
            state->loadDrawState();
        }

        auto &triPosFloats = workload.drawData.triPosFloats;
        auto &triTcFloats = workload.drawData.triTcFloats;
        auto &triColorFloats = workload.drawData.triColorFloats;
        const uint32_t PosFloatsPerVertex = 4;
        const uint32_t TcFloatsPerVertex = 2;
        const uint32_t ColFloatsPerVertex = 4;
        const uint32_t PosFloatsPerTri = PosFloatsPerVertex * 3;
        const uint32_t TcFloatsPerTri = TcFloatsPerVertex * 3;
        const uint32_t ColFloatsPerTri = ColFloatsPerVertex * 3;
        triPosFloats.insert(triPosFloats.end(), pos, pos + triCount * PosFloatsPerTri);
        triTcFloats.insert(triTcFloats.end(), tc, tc + triCount * TcFloatsPerTri);
        triColorFloats.insert(triColorFloats.end(), col, col + triCount * ColFloatsPerTri);
        drawCall.triangleCount += triCount;

        const FixedRect &scissorRect = state->rdp->scissorRectStack[scissorStackSize - 1];
        if (!scissorRect.isNull()) {
            fbPair.scissorRect.merge(scissorRect);

            FixedRect drawRect;
            for (uint32_t i = 0; i < triCount * 3; i++) {
                drawRect.ulx = std::min(drawRect.ulx, int32_t(pos[i * PosFloatsPerVertex + 0] * 4.0f));
                drawRect.uly = std::min(drawRect.uly, int32_t(pos[i * PosFloatsPerVertex + 1] * 4.0f));
                drawRect.lrx = std::max(drawRect.lrx, int32_t(ceilf(pos[i * PosFloatsPerVertex + 0]) * 4.0f));
                drawRect.lry = std::max(drawRect.lry, int32_t(ceilf(pos[i * PosFloatsPerVertex + 1]) * 4.0f));
            }

            const FixedRect intRect = scissorRect.intersection(drawRect);
            if (!intRect.isNull()) {
                fbPair.drawColorRect.merge(intRect);
                if (otherMode.zUpd()) {
                    fbPair.drawDepthRect.merge(intRect);
                }
            }
        }
    }

    void RDP::drawRect(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, int16_t uls, int16_t ult, int16_t dsdx, int16_t dtdy, bool flip) {
        drawRect(ulx, uly, lrx, lry, uls, ult, dsdx, dtdy, flip, extended.global.rect);
    }
    
    void RDP::drawRect(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, int16_t uls, int16_t ult, int16_t dsdx, int16_t dtdy, bool flip, const ExtendedAlignment &extAlignment) {
        // Round down the left and top coordinates in fill mode or copy mode.
        const bool usesFillMode = (otherMode.cycleType() == G_CYC_FILL);
        const bool usesCopyMode = (otherMode.cycleType() == G_CYC_COPY);
        if (usesFillMode || usesCopyMode) {
            ulx &= ~3;
            uly &= ~3;
        }

        // Add global offsets to the coordinates.
        ulx += extAlignment.leftOffset;
        uly += extAlignment.topOffset;
        lrx += extAlignment.rightOffset;
        lry += extAlignment.bottomOffset;

        // There's a very common error in many games where rectangles are incorrectly configured with one
        // less pixel than what's required to fill out the screen because other rectangle methods under
        // different modes require adding an extra coordinate. Since this behavior often breaks detection
        // for widescreen hacks, an enhancement option to fix them is available given they're within the
        // tolerance of one pixel from the scissor coordinates.
        const FixedRect &scissorRect = state->rdp->scissorRectStack[scissorStackSize - 1];
        const bool fixRectLR = state->ext.enhancementConfig->rect.fixRectLR;
        if (!scissorRect.isNull() && fixRectLR) {
            if ((abs(scissorRect.lrx - lrx) <= 4) && (ulx < scissorRect.lrx)) {
                lrx = scissorRect.lrx;
            }

            if ((abs(scissorRect.lry - lry) <= 4) && (uly < scissorRect.lry)) {
                lry = scissorRect.lry;
            }
        }

        const FixedRect drawRect(movedFromOrigin(ulx, extAlignment.leftOrigin), uly, movedFromOrigin(lrx, extAlignment.rightOrigin), lry);
        if (drawRect.isEmpty()) {
            return;
        }

        // BAR draws a black border around its CRT overscan-safe area: four black fill rectangles that
        // together are the complement of the rectangle it renders into (measured with BAR_DBG_RECT
        // during a race: (0,0)-(320,17), (0,224)-(320,240), (0,17)-(21,223) and (296,17)-(320,223)).
        // On a television the bezel hid that margin. On a monitor it is a black frame, and in
        // widescreen it is worse than that: the top and bottom rectangles span the whole scissor
        // width, so RT64 stretches them across the widened frame, while the side ones do not and so
        // are pinned at their 4:3 positions -- black columns standing in the middle of the picture
        // with widened world visible outside them. That is the "island" recorded in KNOWN_ISSUES; the
        // 3D itself already covers the whole frame, the mask is simply painted over it.
        //
        // `patches/viewport_patch.c` already removed the same overscan inset from the game's viewport
        // and scissor. This removes the other half of it, the mask drawn on top. Recognised narrowly:
        // an opaque black fill rectangle, hugging one edge of the scissor, no thicker than a tenth of
        // it. The intro's cinematic letterbox is ~20% of the height and so is deliberately kept.
        // BAR_KEEP_OVERSCAN=1 restores the mask.
        {
            static const bool keepOverscan = std::getenv("BAR_KEEP_OVERSCAN") != nullptr;
            const FixedRect &sc = scissorRectStack[scissorStackSize - 1];
            if (!keepOverscan && (otherMode.cycleType() == G_CYC_FILL) && !sc.isNull() &&
                ((fillColorStack[fillColorStackSize - 1] & 0xFFFF) == 0x0001))
            {
                const int32_t bandX = (sc.lrx - sc.ulx) / 10;
                const int32_t bandY = (sc.lry - sc.uly) / 10;
                const bool left   = (drawRect.ulx <= sc.ulx) && (drawRect.lrx <= sc.ulx + bandX);
                const bool right  = (drawRect.lrx >= sc.lrx) && (drawRect.ulx >= sc.lrx - bandX);
                const bool top    = (drawRect.uly <= sc.uly) && (drawRect.lry <= sc.uly + bandY);
                const bool bottom = (drawRect.lry >= sc.lry) && (drawRect.uly >= sc.lry - bandY);
                if (left || right || top || bottom) {
                    return;
                }
            }
        }

        // BAR_SKIP_WIDE=<pixels>: probe -- drop textured rectangles at least this wide, to find out
        // which on-screen element a class of draw is responsible for. Measured so far: at 150 the
        // track-select screen loses its course-name text, and the racing sky is unaffected (so the
        // sky is not made of wide texture rectangles). See docs/RACING_DRAW_MAP.md.
        {
            static const int skipWide = [] {
                const char *e = std::getenv("BAR_SKIP_WIDE");
                int parsed = 0;
                return ((e != nullptr) && (sscanf(e, "%d", &parsed) == 1)) ? parsed : 0;
            }();
            if ((skipWide > 0) && (otherMode.cycleType() != G_CYC_FILL) &&
                ((drawRect.lrx - drawRect.ulx) >= (skipWide * 4)))
            {
                return;
            }
        }

        // BAR_DBG_RECT=1 logs each DISTINCT 2D rectangle the game draws, in framebuffer pixels, with
        // the cycle type, whether it is textured, and the current fill colour. BAR's black overscan
        // border is drawn as 2D rectangles, and this is what identifies them (position, size and
        // colour) so they can be told apart from the HUD elements drawn beside them.
        {
            static const bool rectDbg = std::getenv("BAR_DBG_RECT") != nullptr;
            if (rectDbg) {
                static std::set<uint64_t> seen;
                const uint64_t key =
                    (uint64_t(uint16_t(drawRect.ulx)) << 48) | (uint64_t(uint16_t(drawRect.uly)) << 32) |
                    (uint64_t(uint16_t(drawRect.lrx)) << 16) | uint64_t(uint16_t(drawRect.lry));
                if (seen.insert(key).second && (seen.size() <= 4000)) {
                    const char *cyc = "2cycle";
                    switch (otherMode.cycleType()) {
                    case G_CYC_1CYCLE: cyc = "1cycle"; break;
                    case G_CYC_COPY:   cyc = "copy";   break;
                    case G_CYC_FILL:   cyc = "fill";   break;
                    default: break;
                    }
                    const FixedRect &sc = scissorRectStack[scissorStackSize - 1];
                    fprintf(stdout, "[rect] px=(%.1f,%.1f)-(%.1f,%.1f) %s tex=%d fill=0x%08X sc=(%.1f,%.1f)-(%.1f,%.1f) fbw=%u\n",
                        drawRect.ulx / 4.0f, drawRect.uly / 4.0f, drawRect.lrx / 4.0f, drawRect.lry / 4.0f,
                        cyc, int(state->drawCall.textureOn), fillColorStack[fillColorStackSize - 1],
                        sc.ulx / 4.0f, sc.uly / 4.0f, sc.lrx / 4.0f, sc.lry / 4.0f, colorImage.width);
                    fflush(stdout);
                }
            }
        }

        // Check if framebuffer pair must be changed.
        checkFramebufferPair();

        // We always flush on any rectangle as we want to have the individual calls.
        state->flush();

        // Change projection to rectangle.
        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        FramebufferPair &fbPair = workload.fbPairs[workload.currentFramebufferPairIndex()];
        if (!fbPair.inProjection(0, Projection::Type::Rectangle)) {
            fbPair.changeProjection(0, Projection::Type::Rectangle);
        }

        if (!scissorRect.isNull()) {
            fbPair.scissorRect.merge(scissorRect);

            const FixedRect intRect = scissorRect.intersection(drawRect);
            if (!intRect.isNull()) {
                fbPair.drawColorRect.merge(intRect);
                if (otherMode.zUpd()) {
                    fbPair.drawDepthRect.merge(intRect);
                }
            }
        }
        
        bool flushedState = state->checkDrawState();

        // We only change these once the draw state has been checked.
        DrawCall &drawCall = state->drawCall;
        drawCall.minWorldMatrix = 0;
        drawCall.maxWorldMatrix = 0;
        drawCall.rect = drawRect;
        drawCall.rectDsdx = dsdx;
        drawCall.rectDtdy = dtdy;
        drawCall.rectLeftOrigin = extAlignment.leftOrigin;
        drawCall.rectRightOrigin = extAlignment.rightOrigin;
        drawCall.rectAspect = extended.global.rectAspect;

        if (flushedState) {
            state->loadDrawState();
        }

        // BAR: anchor the racing HUD to the edges of the widened frame. BAR emits no extended-GBI
        // origins of its own, so without this every 2D element is placed by scaling it about the
        // frame's centre and the whole HUD stays huddled in the middle 4:3 region. Supplying the
        // origins here is the same thing a game built for the extended GBI would do, so everything
        // downstream -- the rectangle's placement, its scissor, the misalignment correction -- already
        // knows what to do with them. See rt64_bar_hud.cpp for how an element is classified.
        //
        // The scissor has to travel with the rectangle: BAR scissors its HUD to the full 320x240
        // screen, which by default converts to the centred 4:3 region, and an element anchored to an
        // edge of the widened frame lands outside it and is clipped away. Giving the scissor the two
        // edge origins widens it to the whole frame for this draw only.
        //
        // This runs after loadDrawState because loadDrawState is what copies the game's own scissor
        // origins into the draw call; marking the attribute changed afterwards is what makes the next
        // draw reload them and so keeps the widened scissor to this one rectangle.
        {
            // classifyRect is called for EVERY 2D rectangle now, not only during a race, because it
            // is also what publishes the frame's elements to the port's HUD inspector and what
            // applies a hud.json tag -- and both of those have to work in the menus. It returns
            // Class::Center and costs a single branch when there is no inspector, no tag table and
            // no race in progress.
            const bool fillCycle = (otherMode.cycleType() == G_CYC_FILL);
            const BarHud::Class cls = BarHud::classifyRect(drawRect, colorImage.width, fillCycle,
                texture.address, state->displayListAddress, state->drawCall.textureOn != 0,
                fillColorStack[fillColorStackSize - 1]);

            switch (cls) {
            case BarHud::Class::Left:
            case BarHud::Class::Right:
                drawCall.rectLeftOrigin = BarHud::leftOriginFor(cls);
                drawCall.rectRightOrigin = BarHud::rightOriginFor(cls);
                drawCall.scissorLeftOrigin = G_EX_ORIGIN_LEFT;
                drawCall.scissorRightOrigin = G_EX_ORIGIN_RIGHT;
                state->updateDrawStatusAttribute(DrawAttribute::Scissor);
                break;
            case BarHud::Class::Stretch:
                // "Do not squeeze this to 4:3", which is not the same as "cover the widened frame":
                // a 320-wide rectangle reaches the frame's full width and a narrower one is widened
                // about its centre by the same factor. The element's own origins are deliberately
                // left alone -- pinning its edges to the frame's edges is what Left and Right do.
                drawCall.rectAspect = G_EX_ASPECT_STRETCH;
                // The scissor has to travel with it, for the same reason it travels with Left and
                // Right: BAR scissors its 2D to the whole 320x240 screen, which RT64 converts to the
                // centred 4:3 region, so anything drawn wider than that is clipped straight back to
                // where it started. Widening the rectangle without widening its scissor is the
                // "half a change looks identical to no change" trap that
                // docs/technical/08-diagnostics-and-methodology.md warns about -- it would have
                // looked exactly like the tag not being applied at all.
                //
                // wave-race-64-recomp does not need this for its own stretch class, which is why it
                // was not here to begin with.
                drawCall.scissorLeftOrigin = G_EX_ORIGIN_LEFT;
                drawCall.scissorRightOrigin = G_EX_ORIGIN_RIGHT;
                state->updateDrawStatusAttribute(DrawAttribute::Scissor);
                break;
            case BarHud::Class::Spill:
                // Placement untouched; only the scissor is widened, for the length of this one
                // rectangle. For an element that is already the right size in the right place and is
                // merely being cut off where the 4:3 frame used to end. BAR scissors a race to
                // x 8..311, so an element drawn from x 0 has eight units hidden even on hardware.
                drawCall.scissorLeftOrigin = G_EX_ORIGIN_LEFT;
                drawCall.scissorRightOrigin = G_EX_ORIGIN_RIGHT;
                state->updateDrawStatusAttribute(DrawAttribute::Scissor);
                break;
            case BarHud::Class::Center:
            default:
                break;
            }
        }

        auto &triPosFloats = workload.drawData.triPosFloats;
        auto &triTcFloats = workload.drawData.triTcFloats;
        auto &triColorFloats = workload.drawData.triColorFloats;
        static const float rectPosFloats[] = {
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 1.0f
        };

        static const float rectColorFloats[] = {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };

        triPosFloats.insert(triPosFloats.end(), rectPosFloats, rectPosFloats + std::size(rectPosFloats));
        triColorFloats.insert(triColorFloats.end(), rectColorFloats, rectColorFloats + std::size(rectColorFloats));

        const int32_t rectWidth = drawRect.width(true, true);
        const int32_t rectHeight = drawRect.height(true, true);
        const int32_t uvWidth = (flip ? rectHeight : rectWidth) << 2;
        const int32_t uvHeight = (flip ? rectWidth : rectHeight) << 2;
        const int32_t lrs = ((uls << 7) + dsdx * uvWidth) >> 7;
        const int32_t lrt = ((ult << 7) + dtdy * uvHeight) >> 7;
        const float vFractionOffset = (uly & 0x3) ? (dtdy >> 5) / 32.0f : 0.0f;
        float u1 = uls / 32.0f;
        float v1 = ult / 32.0f + vFractionOffset;
        float u2 = lrs / 32.0f;
        float v2 = lrt / 32.0f + vFractionOffset;
        triTcFloats.emplace_back(u1);
        triTcFloats.emplace_back(v1);
        triTcFloats.emplace_back(flip ? u1 : u2);
        triTcFloats.emplace_back(flip ? v2 : v1);
        triTcFloats.emplace_back(flip ? u2 : u1);
        triTcFloats.emplace_back(flip ? v1 : v2);
        triTcFloats.emplace_back(u2);
        triTcFloats.emplace_back(v2);
        triTcFloats.emplace_back(flip ? u1 : u2);
        triTcFloats.emplace_back(flip ? v2 : v1);
        triTcFloats.emplace_back(flip ? u2 : u1);
        triTcFloats.emplace_back(flip ? v1 : v2);
        drawCall.triangleCount += 2;

        // Update the tracked texcoords for the used tiles. Use scissor intersection to figure out 
        // what the bounds of the real sampling will be if necessary.
        const bool computeIntersection = !scissorRect.isNull() && !scissorRect.fullyInside(drawRect);
        if (computeIntersection) {
            const FixedRect intersectionRect = scissorRect.intersection(drawRect);
            if (!intersectionRect.isNull()) {
                const int32_t leftPixels = intersectionRect.left(true) - drawRect.left(true);
                const int32_t topPixels = intersectionRect.top(true) - drawRect.top(true);
                const int32_t rightPixels = intersectionRect.right(true) - drawRect.left(true);
                const int32_t bottomPixels = intersectionRect.bottom(true) - drawRect.top(true);
                const float intU1 = (((uls << 7) + dsdx * ((flip ? topPixels : leftPixels) << 2)) >> 7) / 32.0f;
                const float intV1 = (((ult << 7) + dtdy * ((flip ? leftPixels : topPixels) << 2)) >> 7) / 32.0f + vFractionOffset;
                const float intU2 = (((uls << 7) + dsdx * ((flip ? bottomPixels : rightPixels) << 2)) >> 7) / 32.0f;
                const float intV2 = (((ult << 7) + dtdy * ((flip ? rightPixels : bottomPixels) << 2)) >> 7) / 32.0f + vFractionOffset;
                updateCallTexcoords(intU1, intV1);
                updateCallTexcoords(intU2, intV2);
            }
        }
        else {
            updateCallTexcoords(u1, v1);
            updateCallTexcoords(u2, v2);
        }
    }

    void RDP::drawTexRect(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, uint8_t tile, int16_t uls, int16_t ult, int16_t dsdx, int16_t dtdy, bool flip) {
        drawTexRect(ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, flip, extended.global.rect);
    }
    
    void RDP::drawTexRect(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, uint8_t tile, int16_t uls, int16_t ult, int16_t dsdx, int16_t dtdy, bool flip, const ExtendedAlignment &extAlignment) {
#   ifdef LOG_TEXRECT_METHODS
        RT64_LOG_PRINTF("RDP::drawTexRect(ulx %d, uly %d, lrx %d, lry %d, tile %u, uls %d, ult %d, dsdx %d, dtdy %d, flip %u)", ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, flip);
#   endif

        // Check if the texture needs to be updated.
        DrawCall &drawCall = state->drawCall;
        if (!drawCall.textureOn || (drawCall.textureTile != tile) || (drawCall.textureLevels != 1)) {
            drawCall.textureOn = 1;
            drawCall.textureTile = tile;
            drawCall.textureLevels = 1;
            state->updateDrawStatusAttribute(DrawAttribute::Texture);
        }
        
        // Divide dsdx by 4 and add an extra pixel to the edges if it uses copy mode.
        const bool usesCopyMode = (otherMode.cycleType() == G_CYC_COPY);
        if (usesCopyMode) {
            dsdx >>= 2;
            lrx |= 3;
            lry |= 3;
        }

        drawRect(ulx, uly, lrx, lry, uls, ult, dsdx, dtdy, flip, extAlignment);
    }

    void RDP::updateCallTexcoords(float u, float v) {
        const int workloadCursor = state->ext.workloadQueue->writeCursor;
        Workload &workload = state->ext.workloadQueue->workloads[workloadCursor];
        for (uint32_t t = 0; t < state->drawCall.tileCount; t++) {
            DrawCallTile &callTile = workload.drawData.callTiles[state->drawCall.tileIndex + t];
            callTile.minTexcoord.x = std::min(callTile.minTexcoord.x, int(u));
            callTile.minTexcoord.y = std::min(callTile.minTexcoord.y, int(v));
            callTile.maxTexcoord.x = std::max(callTile.maxTexcoord.x, int(ceilf(u)));
            callTile.maxTexcoord.y = std::max(callTile.maxTexcoord.y, int(ceilf(v)));
        }
    }
};
