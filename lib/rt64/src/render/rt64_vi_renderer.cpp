//
// RT64
//

#include "rt64_vi_renderer.h"

#include <chrono>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "shared/rt64_hlsl.h"
#include "shared/rt64_video_interface.h"

namespace RT64 {
    // VIRenderer

    VIRenderer::VIRenderer() { }

    VIRenderer::~VIRenderer() { }

    inline hlslpp::float2 computeHDSize(hlslpp::float2 sdSize, hlslpp::float2 resolutionScale, uint32_t downsamplingScale) {
        return (sdSize * resolutionScale) / float(downsamplingScale);
    }

    inline hlslpp::float2 fromSDtoHD(hlslpp::float2 coordinate, hlslpp::float2 sdSize, hlslpp::float2 hdSize) {
        const hlslpp::float2 relativeScale = hdSize / sdSize;
        return coordinate * relativeScale;
    }

    // BAR: which part of the framebuffer actually holds the picture, per screen.
    //
    // BAR does not draw into all of its 320x240 framebuffer on every screen. Racing is rendered into
    // a 275x207 rectangle inset at (22,17) -- a CRT overscan-safe area that a television bezel used
    // to cover -- while full-screen 2D (logos, menus) uses the whole 320x240, and the intro
    // cinematic uses a letterboxed rectangle of its own. Presenting the whole framebuffer shows the
    // unused margin as black bars; presenting one FIXED crop is wrong too, because it is only
    // correct for whichever screen it was measured on.
    //
    // So the region is observed rather than assumed: the game tells us, every frame, through the
    // scissor rectangles it emits. Of the rectangles seen in a frame we take the SMALLEST one that
    // still covers at least BAR_CONTENT_MIN_AREA of the framebuffer (default 50%). Smallest, because
    // during racing the game sets both its inset rectangle and a full-screen one, and the inset is
    // the picture; area-gated, because small scissors are used to clip individual elements and must
    // never be mistaken for the frame.
    //
    //   BAR_CONTENT_INSET=0          present the whole framebuffer (bars and all)
    //   BAR_CONTENT_INSET=l,t,r,b    force a fixed rectangle, in 320x240 pixels
    //   BAR_CONTENT_MIN_AREA=<0..1>  change the area gate
    namespace {
        struct ContentRectPx { float left, top, right, bottom; };

        std::mutex g_contentMutex;
        // Smallest qualifying rectangle seen in the current sampling window, and the rectangle
        // currently being presented. Seeded full-screen so the first frames present normally.
        //
        // Sampling over a short WINDOW rather than a single frame is deliberate.
        // getViewportAndScissor runs more than once per presented frame (the present path and the
        // cursor mapping both call it, and interpolated frames add more), so an earlier version that
        // consumed the accumulated candidate on every call alternated between the game's two
        // rectangles on consecutive calls -- the image pulsed between zoomed and unzoomed at frame
        // rate. Reads must therefore never consume the accumulator; only the window rollover updates
        // what is presented.
        ContentRectPx g_windowSmallest{ 0.0f, 0.0f, 320.0f, 240.0f };
        bool g_windowValid = false;
        std::chrono::steady_clock::time_point g_windowStart{};
        ContentRectPx g_contentLatched{ 0.0f, 0.0f, 320.0f, 240.0f };
        constexpr std::chrono::milliseconds kContentWindow{ 250 };

        float contentMinArea() {
            static const float v = [] {
                const char *e = std::getenv("BAR_CONTENT_MIN_AREA");
                float parsed = 0.0f;
                if ((e != nullptr) && (sscanf(e, "%f", &parsed) == 1) && (parsed > 0.0f) && (parsed <= 1.0f)) {
                    return parsed;
                }
                return 0.5f;
            }();
            return v;
        }

        // -1 = observe per frame (default), 0 = whole framebuffer, 1 = fixed rectangle in g_forcedRect.
        ContentRectPx g_forcedRect{ 0.0f, 0.0f, 320.0f, 240.0f };
        // DEFAULT IS OFF: present the whole framebuffer, exactly as RT64 does upstream.
        //
        // Cropping to the game's drawn region is an enhancement layered on top of the aspect-ratio
        // handling, and it must stay off until plain 4:3 presentation is correct on its own --
        // otherwise a wrong image cannot be attributed to either mechanism. Turn it on with
        // BAR_CONTENT_INSET=auto once the baseline is signed off.
        int contentMode() {
            static const int mode = [] {
                const char *e = std::getenv("BAR_CONTENT_INSET");
                if (e == nullptr) {
                    return 0;
                }
                if (strcmp(e, "auto") == 0) {
                    return -1;
                }
                if (strcmp(e, "0") == 0) {
                    return 0;
                }
                float l, t, r, b;
                if (sscanf(e, "%f,%f,%f,%f", &l, &t, &r, &b) == 4) {
                    g_forcedRect = { l, t, r, b };
                    return 1;
                }
                return 0;
            }();
            return mode;
        }
    }

    void VIRenderer::barReportGameScissor(float ulx, float uly, float lrx, float lry) {
        if (contentMode() != -1) {
            return;
        }

        const float w = lrx - ulx;
        const float h = lry - uly;
        if ((w <= 0.0f) || (h <= 0.0f)) {
            return;
        }

        // Area gate, against the framebuffer this game uses. 320x240 is the reference; a rectangle
        // is only a candidate for "the frame" if it covers most of it.
        if ((w * h) < (contentMinArea() * 320.0f * 240.0f)) {
            return;
        }

        const std::scoped_lock lock(g_contentMutex);
        const auto now = std::chrono::steady_clock::now();

        // Roll the window over: whatever was smallest across the last kContentWindow becomes the
        // rectangle to present, and a fresh window starts. A screen holds its rectangle for many
        // frames, so this settles immediately and only moves on a real screen change.
        if ((now - g_windowStart) >= kContentWindow) {
            if (g_windowValid) {
                g_contentLatched = g_windowSmallest;
            }
            g_windowStart = now;
            g_windowValid = false;
        }

        if (!g_windowValid || ((w * h) < ((g_windowSmallest.right - g_windowSmallest.left) * (g_windowSmallest.bottom - g_windowSmallest.top)))) {
            g_windowSmallest = { ulx, uly, lrx, lry };
            g_windowValid = true;
        }
    }

    // Latches the frame's candidate and returns the rectangle to present, as fractions of the
    // framebuffer so it stays correct whatever size that is.
    static ContentRectPx barGetContentRect() {
        const int mode = contentMode();
        if (mode == 0) {
            return { 0.0f, 0.0f, 1.0f, 1.0f };
        }
        if (mode == 1) {
            return { g_forcedRect.left / 320.0f, g_forcedRect.top / 240.0f, g_forcedRect.right / 320.0f, g_forcedRect.bottom / 240.0f };
        }

        ContentRectPx px;
        {
            // Read only -- see the note on g_windowSmallest for why this must not consume.
            const std::scoped_lock lock(g_contentMutex);
            px = g_contentLatched;
        }

        static const bool dbg = std::getenv("BAR_DBG_CONTENT") != nullptr;
        if (dbg) {
            static ContentRectPx lastPrinted{ -1.0f, -1.0f, -1.0f, -1.0f };
            if ((px.left != lastPrinted.left) || (px.top != lastPrinted.top) || (px.right != lastPrinted.right) || (px.bottom != lastPrinted.bottom)) {
                lastPrinted = px;
                fprintf(stderr, "[content] presenting (%.1f,%.1f)-(%.1f,%.1f)  %.0fx%.0f\n",
                    px.left, px.top, px.right, px.bottom, px.right - px.left, px.bottom - px.top);
                fflush(stderr);
            }
        }

        return { px.left / 320.0f, px.top / 240.0f, px.right / 320.0f, px.bottom / 240.0f };
    }

    // Maps an HD-space point into the window, scaling and centring on `refCenter`/`refSize` -- the
    // region that should end up filling the window. Passing the whole image as the reference gives
    // the original behaviour; passing the content rect above crops the game's own black margin.
    inline hlslpp::float2 fromHDtoWindowRef(hlslpp::float2 coordinate, hlslpp::float2 refCenter, hlslpp::float2 refSize, hlslpp::float2 windowSize, UserConfiguration::PresentFillMode fillMode) {
        const hlslpp::float2 windowCenter = windowSize / 2;
        const hlslpp::float2 relativeCoordinate = { coordinate.x - refCenter.x, coordinate.y - refCenter.y };

        const float scaleX = windowSize.x / refSize.x;
        const float scaleY = windowSize.y / refSize.y;

        if (fillMode == UserConfiguration::PresentFillMode::Stretch) {
            return { windowCenter.x + relativeCoordinate.x * scaleX, windowCenter.y + relativeCoordinate.y * scaleY };
        }

        const float minScale = (scaleX < scaleY) ? scaleX : scaleY;
        const float maxScale = (scaleX > scaleY) ? scaleX : scaleY;
        const float relativeScale = (fillMode == UserConfiguration::PresentFillMode::Crop) ? maxScale : minScale;

        return windowCenter + relativeCoordinate * relativeScale;
    }

    inline hlslpp::float2 fromHDtoWindow(hlslpp::float2 coordinate, hlslpp::float2 hdSize, hlslpp::float2 windowSize, UserConfiguration::PresentFillMode fillMode) {
        const hlslpp::float2 hdCenter = hdSize / 2;
        const hlslpp::float2 windowCenter = windowSize / 2;
        const hlslpp::float2 relativeCoordinate = { coordinate.x - hdCenter.x, coordinate.y - hdCenter.y };

        const float scaleX = windowSize.x / hdSize.x;
        const float scaleY = windowSize.y / hdSize.y;

        // Stretch: scale each axis independently so the image exactly fills the window
        // (aspect distorted, but no black bars on either side).
        if (fillMode == UserConfiguration::PresentFillMode::Stretch) {
            return { windowCenter.x + relativeCoordinate.x * scaleX, windowCenter.y + relativeCoordinate.y * scaleY };
        }

        // Uniform scale, preserving the 4:3 aspect:
        //   Pillarbox (contain, min-scale) — the whole image fits; the long axis gets black bars.
        //     Wider-than-4:3 window -> left/right pillars; taller window -> top/bottom letterbox.
        //     This is RT64's original present behavior (the BAR default).
        //   Crop      (cover,   max-scale) — the image fills the window; the overflow is cut by the
        //     scissor clamp in getViewportAndScissor, so there are no bars.
        const float minScale = (scaleX < scaleY) ? scaleX : scaleY;
        const float maxScale = (scaleX > scaleY) ? scaleX : scaleY;
        const float relativeScale = (fillMode == UserConfiguration::PresentFillMode::Crop) ? maxScale : minScale;

        return windowCenter + relativeCoordinate * relativeScale;
    }

    void VIRenderer::render(const RenderParams &p) {
        const ShaderRecord *shader = nullptr;
        const RenderSampler *sampler = nullptr;
        switch (p.filtering) {
        case UserConfiguration::Filtering::Nearest:
            shader = &p.shaderLibrary->videoInterfaceNearest;
            sampler = p.shaderLibrary->samplerLibrary.nearest.borderBorder.get();
            break;
        case UserConfiguration::Filtering::AntiAliasedPixelScaling:
            shader = &p.shaderLibrary->videoInterfacePixel;
            sampler = p.shaderLibrary->samplerLibrary.linear.borderBorder.get();
            break;
        case UserConfiguration::Filtering::Linear:
        default:
            shader = &p.shaderLibrary->videoInterfaceLinear;
            sampler = p.shaderLibrary->samplerLibrary.linear.borderBorder.get();
            break;
        }

        if ((descriptorSet == nullptr) || (descriptorSetSampler != sampler)) {
            descriptorSet = std::make_unique<VideoInterfaceDescriptorSet>(sampler, p.device);
            descriptorSetSampler = sampler;
        }

        descriptorSet->setTexture(descriptorSet->gInput, p.texture, RenderTextureLayout::SHADER_READ);

        RenderViewport viewport;
        RenderRect scissor;
        getViewportAndScissor(p.swapChain, *p.vi, p.resolutionScale, p.downsamplingScale, p.removeBlackBorders, p.fillMode, viewport, scissor);
        p.commandList->setViewports(viewport);
        p.commandList->setScissors(scissor);

        interop::VideoInterfaceCB pushConstants;
        pushConstants.videoResolution = computeHDSize(hlslpp::float2(p.vi->fbSize()), p.resolutionScale, p.downsamplingScale);
        pushConstants.textureResolution = { float(p.textureWidth), float(p.textureHeight) };
        pushConstants.gamma = p.vi->gamma();
        pushConstants.divotFilter = p.divotFilter ? 1u : 0u;   // BAR seam fix: N64 VI divot median
        pushConstants.divotThreshold = p.divotThreshold;

        p.commandList->setPipeline(shader->pipeline.get());
        p.commandList->setGraphicsPipelineLayout(shader->pipelineLayout.get());
        p.commandList->setGraphicsDescriptorSet(descriptorSet->get(), 0);
        p.commandList->setGraphicsPushConstants(0, &pushConstants);
        p.commandList->setVertexBuffers(0, nullptr, 0, nullptr);
        p.commandList->drawInstanced(3, 1, 0, 0);
    }

    void VIRenderer::getViewportAndScissor(const RenderSwapChain *swapChain, const VI &vi, hlslpp::float2 resolutionScale, uint32_t downsamplingScale, bool removeBlackBorders, UserConfiguration::PresentFillMode fillMode, RenderViewport &viewport, RenderRect &scissor) {
        // We define three different coordinate spaces to work with to translate the VI parameters into the Window.
        //
        // VideoSD: This corresponds to the SD TV Scanline space, which is what the VI natively works on.
        // 
        // VideoHD: This corresponds to what the imaginary "HD" TV would be if it supported the amount of scanlines
        // desired by the resolution scale (divided by the downsampling scale) and a wider aspect ratio.
        // 
        // Window: The native coordinate space of the device's render target.
        //
        // The provided buffer doesn't necessarily have the same dimensions that the VI will sample to display it 
        // on the screen. To work around that, the viewport the buffer will be drawn in will be expanded so only
        // the region of interest is rendered. A scissor will cut it off correctly according to the coordinates
        // specified by the VI.
        const hlslpp::float2 sdSize = removeBlackBorders ? hlslpp::float2(vi.fbSize()) : hlslpp::float2(320.0f, 240.0f);
        const hlslpp::float2 hdSize = computeHDSize(sdSize, resolutionScale, downsamplingScale);
        const hlslpp::float2 windowSize = { float(swapChain->getWidth()), float(swapChain->getHeight()) };

        // Query the VI for the current rendering area.
        hlslpp::float4 viViewRect = vi.viewRectangle() * sdSize.xyxy;
        hlslpp::float4 viCropRect = vi.cropRectangle() * sdSize.xyxy;

        // The reference region that should fill the window: the part of the framebuffer the game
        // actually draws into, rather than the whole thing. See getContentInset().
        const ContentRectPx inset = barGetContentRect();
        const hlslpp::float2 contentTopLeftHD = fromSDtoHD({ sdSize.x * inset.left, sdSize.y * inset.top }, sdSize, hdSize);
        const hlslpp::float2 contentBottomRightHD = fromSDtoHD({ sdSize.x * inset.right, sdSize.y * inset.bottom }, sdSize, hdSize);
        const hlslpp::float2 refSize = contentBottomRightHD - contentTopLeftHD;
        const hlslpp::float2 refCenter = (contentBottomRightHD + contentTopLeftHD) / 2;

        // Scale all the rectangles to the space of the Window.
        hlslpp::float2 topLeftViewport = fromSDtoHD({ float(viViewRect.x), float(viViewRect.y) }, sdSize, hdSize);
        hlslpp::float2 bottomRightViewport = fromSDtoHD({ float(viViewRect.x + viViewRect.z), float(viViewRect.y + viViewRect.w) }, sdSize, hdSize);
        topLeftViewport = fromHDtoWindowRef(topLeftViewport, refCenter, refSize, windowSize, fillMode);
        bottomRightViewport = fromHDtoWindowRef(bottomRightViewport, refCenter, refSize, windowSize, fillMode);

        hlslpp::float2 topLeftScissor = fromSDtoHD({ float(viCropRect.x), float(viCropRect.y) }, sdSize, hdSize);
        hlslpp::float2 bottomRightScissor = fromSDtoHD({ float(viCropRect.x + viCropRect.z), float(viCropRect.y + viCropRect.w) }, sdSize, hdSize);
        topLeftScissor = fromHDtoWindowRef(topLeftScissor, refCenter, refSize, windowSize, fillMode);
        bottomRightScissor = fromHDtoWindowRef(bottomRightScissor, refCenter, refSize, windowSize, fillMode);

        viewport = RenderViewport(topLeftViewport.x, topLeftViewport.y, bottomRightViewport.x - topLeftViewport.x, bottomRightViewport.y - topLeftViewport.y);

        // BAR_DBG_VI=1 reports how the VI's own rectangles turn into the final on-screen rectangle.
        // Black bars in 4:3 can come from three different places -- the VI's view rect, its crop rect,
        // or the window fit -- and they are indistinguishable from a screenshot, so print all three.
        // Rate-limited to once a second; the values are constant for a given screen.
        {
            static const bool viDbg = std::getenv("BAR_DBG_VI") != nullptr;
            if (viDbg) {
                static std::chrono::steady_clock::time_point lastPrint{};
                const auto now = std::chrono::steady_clock::now();
                if (now - lastPrint > std::chrono::seconds(1)) {
                    lastPrint = now;
                    const hlslpp::float4 vr = vi.viewRectangle();
                    const hlslpp::float4 cr = vi.cropRectangle();
                    fprintf(stderr,
                        "[vi] fb=%.0fx%.0f sd=%.0fx%.0f hd=%.0fx%.0f win=%.0fx%.0f | viewRect=(%.4f,%.4f,%.4f,%.4f) "
                        "cropRect=(%.4f,%.4f,%.4f,%.4f) | viewport=(%.0f,%.0f %.0fx%.0f) scissorPx=(%.0f,%.0f)-(%.0f,%.0f)\n",
                        float(vi.fbSize().x), float(vi.fbSize().y),
                        float(sdSize.x), float(sdSize.y), float(hdSize.x), float(hdSize.y),
                        float(windowSize.x), float(windowSize.y),
                        float(vr.x), float(vr.y), float(vr.z), float(vr.w),
                        float(cr.x), float(cr.y), float(cr.z), float(cr.w),
                        viewport.x, viewport.y, viewport.width, viewport.height,
                        float(topLeftScissor.x), float(topLeftScissor.y),
                        float(bottomRightScissor.x), float(bottomRightScissor.y));
                    fflush(stderr);
                }
            }
        }

        // Clamp the scissor to the window bounds. For Crop the image overflows the window, so this
        // trims the overflow to give a clean fill; for Pillarbox/Stretch the scissor already lies within
        // the window, so this is a no-op that just guards against off-by-one rounding at the edges.
        const long winW = lround(windowSize.x);
        const long winH = lround(windowSize.y);
        auto clampToWindow = [](long v, long hi) -> int32_t { return int32_t(v < 0 ? 0 : (v > hi ? hi : v)); };
        scissor = RenderRect(
            clampToWindow(lround(topLeftScissor.x), winW),
            clampToWindow(lround(topLeftScissor.y), winH),
            clampToWindow(lround(bottomRightScissor.x), winW),
            clampToWindow(lround(bottomRightScissor.y), winH));
    }
};