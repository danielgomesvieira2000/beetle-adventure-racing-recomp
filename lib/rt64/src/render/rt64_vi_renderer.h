//
// RT64
//

#pragma once

#include "common/rt64_user_configuration.h"
#include "hle/rt64_vi.h"

#include "rt64_descriptor_sets.h"
#include "rt64_shader_library.h"

namespace RT64 {
    struct VIRenderer {
        std::unique_ptr<VideoInterfaceDescriptorSet> descriptorSet;
        const RenderSampler *descriptorSetSampler = nullptr;

        struct RenderParams {
            RenderDevice *device = nullptr;
            RenderCommandList *commandList = nullptr;
            RenderTexture *texture = nullptr;
            const RenderSwapChain *swapChain = nullptr;
            const ShaderLibrary *shaderLibrary = nullptr;
            RenderFormat textureFormat = RenderFormat::UNKNOWN;
            hlslpp::float2 resolutionScale;
            uint32_t downsamplingScale = 0;
            uint32_t textureWidth = 0;
            uint32_t textureHeight = 0;
            UserConfiguration::Filtering filtering = UserConfiguration::Filtering::Linear;
            const VI *vi = nullptr;
            bool removeBlackBorders = false;
            // BAR: how the 4:3 image is fit to the window. Pillarbox (default) preserves the original
            // letterbox/pillarbox present; Crop fills + crops; Stretch fills + distorts. See fromHDtoWindow.
            UserConfiguration::PresentFillMode fillMode = UserConfiguration::PresentFillMode::Pillarbox;
            bool divotFilter = false;       // BAR seam fix: apply the VI divot median this present
            float divotThreshold = 0.12f;   // divot outlier gate: higher fills more seams (tolerates surface
                                            // variation across the crack), too high eats 1px detail.
                                            // BAR_DIVOT_THRESHOLD overrides at runtime.
        };

        VIRenderer();
        ~VIRenderer();
        void render(const RenderParams &p);
        // BAR: report a scissor rectangle the GAME set, in framebuffer pixels, so the present stage
        // can work out which part of the framebuffer actually holds the picture on this screen.
        // Different screens draw into different regions (see barGetContentRect), so this has to be
        // observed per frame rather than assumed.
        static void barReportGameScissor(float ulx, float uly, float lrx, float lry);

        static void getViewportAndScissor(const RenderSwapChain *swapChain, const VI &vi, hlslpp::float2 resolutionScale, uint32_t downsamplingScale, bool removeBlackBorders, UserConfiguration::PresentFillMode fillMode, RenderViewport &viewport, RenderRect &scissor);
    };
};