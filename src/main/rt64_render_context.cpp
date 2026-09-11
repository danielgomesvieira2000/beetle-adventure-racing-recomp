// src/main/rt64_render_context.cpp — RT64-backed RendererContext for beetle-adventure-racing-recomp.
//
// Adapted from Zelda64Recompiled (src/main/rt64_render_context.cpp, MIT License), reduced to the
// core renderer lifecycle (no mods / HD texture packs / UI hooks) and retargeted to this repo's
// pinned lib/rt64 (RT64 @ f0728a2) + lib/N64ModernRuntime. ultramodern drives this interface:
// send_dl() per graphics task (M_GFXTASK) and update_screen() per VI interrupt.

#include <memory>
#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef HLSL_CPU
#  define HLSL_CPU
#endif
#include "hle/rt64_application.h"

#include "ultramodern/ultramodern.hpp"
#include "ultramodern/config.hpp"
#include "ultramodern/renderer_context.hpp"

// Cross-TU signals consumed by main.cpp's deferred start_game().
// g_bar_renderer_ready: RT64 finished setup. g_bar_vi_ticked: the renderer has presented at least
// one frame (update_screen), which means the VI thread has ticked and seeded a dummy OSViMode — so
// it's safe to start the game without racing the VI thread into a null-mode deref.
std::atomic<bool> g_bar_renderer_ready{false};
std::atomic<bool> g_bar_vi_ticked{false};

namespace {

// RT64's Core wants pointers to SP DMEM/IMEM and the RDP/MI registers. Driven in HLE mode through
// send_dl(), these only need valid backing storage — ultramodern owns the real interrupt state,
// and the VI registers are bridged from ultramodern below via get_vi_regs().
uint8_t  s_dmem[0x1000];
uint8_t  s_imem[0x1000];
uint32_t s_mi_intr_reg = 0;
uint32_t s_dpc_start_reg = 0, s_dpc_end_reg = 0, s_dpc_current_reg = 0, s_dpc_status_reg = 0;
uint32_t s_dpc_clock_reg = 0, s_dpc_bufbusy_reg = 0, s_dpc_pipebusy_reg = 0, s_dpc_tmem_reg = 0;

void dummy_check_interrupts() {}

RT64::UserConfiguration::AspectRatio to_rt64(ultramodern::renderer::AspectRatio o) {
    switch (o) {
        case ultramodern::renderer::AspectRatio::Original: return RT64::UserConfiguration::AspectRatio::Original;
        case ultramodern::renderer::AspectRatio::Expand:   return RT64::UserConfiguration::AspectRatio::Expand;
        case ultramodern::renderer::AspectRatio::Manual:   return RT64::UserConfiguration::AspectRatio::Manual;
        default:                                           return RT64::UserConfiguration::AspectRatio::OptionCount;
    }
}
RT64::UserConfiguration::Antialiasing to_rt64(ultramodern::renderer::Antialiasing o) {
    switch (o) {
        case ultramodern::renderer::Antialiasing::None:   return RT64::UserConfiguration::Antialiasing::None;
        case ultramodern::renderer::Antialiasing::MSAA2X: return RT64::UserConfiguration::Antialiasing::MSAA2X;
        case ultramodern::renderer::Antialiasing::MSAA4X: return RT64::UserConfiguration::Antialiasing::MSAA4X;
        case ultramodern::renderer::Antialiasing::MSAA8X: return RT64::UserConfiguration::Antialiasing::MSAA8X;
        default:                                          return RT64::UserConfiguration::Antialiasing::OptionCount;
    }
}
RT64::UserConfiguration::RefreshRate to_rt64(ultramodern::renderer::RefreshRate o) {
    switch (o) {
        case ultramodern::renderer::RefreshRate::Original: return RT64::UserConfiguration::RefreshRate::Original;
        case ultramodern::renderer::RefreshRate::Display:  return RT64::UserConfiguration::RefreshRate::Display;
        case ultramodern::renderer::RefreshRate::Manual:   return RT64::UserConfiguration::RefreshRate::Manual;
        default:                                           return RT64::UserConfiguration::RefreshRate::OptionCount;
    }
}
RT64::UserConfiguration::InternalColorFormat to_rt64(ultramodern::renderer::HighPrecisionFramebuffer o) {
    switch (o) {
        case ultramodern::renderer::HighPrecisionFramebuffer::Off:  return RT64::UserConfiguration::InternalColorFormat::Standard;
        case ultramodern::renderer::HighPrecisionFramebuffer::On:   return RT64::UserConfiguration::InternalColorFormat::High;
        case ultramodern::renderer::HighPrecisionFramebuffer::Auto: return RT64::UserConfiguration::InternalColorFormat::Automatic;
        default:                                                    return RT64::UserConfiguration::InternalColorFormat::OptionCount;
    }
}
RT64::UserConfiguration::DivotFilter to_rt64(ultramodern::renderer::DivotFilter o) {
    switch (o) {
        case ultramodern::renderer::DivotFilter::Auto: return RT64::UserConfiguration::DivotFilter::Auto;
        case ultramodern::renderer::DivotFilter::On:   return RT64::UserConfiguration::DivotFilter::On;
        case ultramodern::renderer::DivotFilter::Off:  return RT64::UserConfiguration::DivotFilter::Off;
        default:                                       return RT64::UserConfiguration::DivotFilter::OptionCount;
    }
}
RT64::UserConfiguration::PresentFillMode to_rt64(ultramodern::renderer::PresentFillMode o) {
    switch (o) {
        case ultramodern::renderer::PresentFillMode::Pillarbox: return RT64::UserConfiguration::PresentFillMode::Pillarbox;
        case ultramodern::renderer::PresentFillMode::Crop:      return RT64::UserConfiguration::PresentFillMode::Crop;
        case ultramodern::renderer::PresentFillMode::Stretch:   return RT64::UserConfiguration::PresentFillMode::Stretch;
        default:                                                return RT64::UserConfiguration::PresentFillMode::OptionCount;
    }
}

void set_application_user_config(RT64::Application* app, const ultramodern::renderer::GraphicsConfig& config) {
    // Render-resolution scale + supersampling. ds_option is the SSAA (downsample) factor: the scene is
    // rendered at scale×ss the N64's native framebuffer, then downsampled by ss for the final image, so
    // the net internal resolution is `scale` with ss× supersampled anti-aliasing. Auto tracks the window.
    const int ss = std::clamp(config.ds_option, 1, 4);
    int  scale = 1;
    bool window_scale = false;
    switch (config.res_option) {
        case ultramodern::renderer::Resolution::Auto:       window_scale = true; break;
        case ultramodern::renderer::Resolution::Original:   scale = 1; break;
        case ultramodern::renderer::Resolution::Original2x: scale = 2; break;
        case ultramodern::renderer::Resolution::Native3x:   scale = 3; break;
        case ultramodern::renderer::Resolution::Native4x:   scale = 4; break;
        case ultramodern::renderer::Resolution::Native6x:   scale = 6; break;
        case ultramodern::renderer::Resolution::Native8x:   scale = 8; break;
        default:                                            scale = 1; break;
    }

    if (window_scale) {
        app->userConfig.resolution           = RT64::UserConfiguration::Resolution::WindowIntegerScale;
        app->userConfig.downsampleMultiplier = ss;
    } else {
        app->userConfig.resolution           = RT64::UserConfiguration::Resolution::Manual;
        // Clamp the render multiplier to RT64's hard limit (32) — 8x scale × 4x SSAA hits exactly 32.
        app->userConfig.resolutionMultiplier =
            std::min<double>(double(scale) * ss, RT64::UserConfiguration::ResolutionMultiplierLimit);
        app->userConfig.downsampleMultiplier = ss;
    }

    switch (config.hr_option) {
        default:
        case ultramodern::renderer::HUDRatioMode::Original:
            app->userConfig.extAspectRatio = RT64::UserConfiguration::AspectRatio::Original;
            break;
        case ultramodern::renderer::HUDRatioMode::Clamp16x9:
            app->userConfig.extAspectRatio = RT64::UserConfiguration::AspectRatio::Manual;
            app->userConfig.extAspectTarget = 16.0 / 9.0;
            break;
        case ultramodern::renderer::HUDRatioMode::Full:
            app->userConfig.extAspectRatio = RT64::UserConfiguration::AspectRatio::Expand;
            break;
    }

    app->userConfig.aspectRatio         = to_rt64(config.ar_option);
    app->userConfig.antialiasing        = to_rt64(config.msaa_option);
    app->userConfig.refreshRate         = to_rt64(config.rr_option);
    app->userConfig.refreshRateTarget   = config.rr_manual_value;
    app->userConfig.internalColorFormat = to_rt64(config.hpfb_option);
    app->userConfig.divotFilter         = to_rt64(config.divot_option);   // BAR seam fix (2B): live VI divot toggle
    app->userConfig.presentFillMode     = to_rt64(config.pfm_option);     // BAR letterbox: live window-fit (pillarbox/crop/stretch)
    app->userConfig.displayBuffering    = RT64::UserConfiguration::DisplayBuffering::Triple;

    // BAR_DBG_GFX=1 reports what actually reached RT64. Widescreen depends on aspectRatio==Expand
    // (1) surviving into RT64's userConfig, so print it rather than trusting the JSON on disk.
    if (std::getenv("BAR_DBG_GFX") != nullptr) {
        std::fprintf(stderr,
            "[gfx] aspectRatio=%d extAspectRatio=%d extAspectTarget=%.4f resolution=%d resMult=%.2f "
            "downMult=%d presentFillMode=%d antialiasing=%d refreshRate=%d\n",
            int(app->userConfig.aspectRatio), int(app->userConfig.extAspectRatio),
            app->userConfig.extAspectTarget, int(app->userConfig.resolution),
            app->userConfig.resolutionMultiplier, int(app->userConfig.downsampleMultiplier),
            int(app->userConfig.presentFillMode), int(app->userConfig.antialiasing),
            int(app->userConfig.refreshRate));
        std::fflush(stderr);
    }
}

ultramodern::renderer::SetupResult map_setup_result(RT64::Application::SetupResult r) {
    switch (r) {
        case RT64::Application::SetupResult::Success:                  return ultramodern::renderer::SetupResult::Success;
        case RT64::Application::SetupResult::DynamicLibrariesNotFound: return ultramodern::renderer::SetupResult::DynamicLibrariesNotFound;
        case RT64::Application::SetupResult::InvalidGraphicsAPI:       return ultramodern::renderer::SetupResult::InvalidGraphicsAPI;
        case RT64::Application::SetupResult::GraphicsAPINotFound:      return ultramodern::renderer::SetupResult::GraphicsAPINotFound;
        case RT64::Application::SetupResult::GraphicsDeviceNotFound:   return ultramodern::renderer::SetupResult::GraphicsDeviceNotFound;
    }
    return ultramodern::renderer::SetupResult::GraphicsDeviceNotFound;
}

ultramodern::renderer::GraphicsApi map_graphics_api(RT64::UserConfiguration::GraphicsAPI api) {
    switch (api) {
        case RT64::UserConfiguration::GraphicsAPI::D3D12:  return ultramodern::renderer::GraphicsApi::D3D12;
        case RT64::UserConfiguration::GraphicsAPI::Vulkan: return ultramodern::renderer::GraphicsApi::Vulkan;
        case RT64::UserConfiguration::GraphicsAPI::Metal:  return ultramodern::renderer::GraphicsApi::Metal;
        default:                                           return ultramodern::renderer::GraphicsApi::Auto;
    }
}

class RT64Context final : public ultramodern::renderer::RendererContext {
public:
    RT64Context(uint8_t* rdram, ultramodern::renderer::WindowHandle window_handle, bool debug);
    ~RT64Context() override = default;

    bool valid() override { return app != nullptr; }
    bool update_config(const ultramodern::renderer::GraphicsConfig& old_config,
                       const ultramodern::renderer::GraphicsConfig& new_config) override;
    void enable_instant_present() override;
    void send_dl(const OSTask* task) override;
    // Added as a pure virtual by an upstream N64ModernRuntime change: the runtime asks the renderer
    // to emit a minimal RDP workload targeting a framebuffer address, so a game that has not yet
    // submitted a display list still produces something presentable.
    void send_dummy_workload(uint32_t fb_address) override;
    void update_screen() override;
    void shutdown() override;
    uint32_t get_display_framerate() const override;
    float get_resolution_scale() const override;

private:
    std::unique_ptr<RT64::Application> app;

    // MSAA + resolution are applied only via RT64's *validated* setup path; changing them live is
    // fragile and crashes: RT64's runtime updateMultisampling() skips the device sample-count check
    // that setup does (rt64_application.cpp:336-342 vs :527), and live render-target reallocation is
    // brittle. We snapshot the live RT64 values after setup and re-assert them on every runtime config
    // update so those two settings never change until the next launch — the chosen values still persist
    // to graphics.json (the menu saves them) and take effect on restart through the safe setup path.
    RT64::UserConfiguration::Antialiasing m_live_aa{};
    RT64::UserConfiguration::Resolution   m_live_resolution{};
    double m_live_res_mult  = 1.0;
    int    m_live_down_mult = 1;
};

RT64Context::RT64Context(uint8_t* rdram, ultramodern::renderer::WindowHandle window_handle, bool debug) {
    static unsigned char dummy_rom_header[0x40] = {};

    RT64::Application::Core core{};
#if defined(_WIN32)
    core.window = window_handle.window;
#else
    core.window = window_handle;
#endif
    core.checkInterrupts = dummy_check_interrupts;
    core.HEADER = dummy_rom_header;
    core.RDRAM  = rdram;
    core.DMEM   = s_dmem;
    core.IMEM   = s_imem;
    core.MI_INTR_REG      = &s_mi_intr_reg;
    core.DPC_START_REG    = &s_dpc_start_reg;
    core.DPC_END_REG      = &s_dpc_end_reg;
    core.DPC_CURRENT_REG  = &s_dpc_current_reg;
    core.DPC_STATUS_REG   = &s_dpc_status_reg;
    core.DPC_CLOCK_REG    = &s_dpc_clock_reg;
    core.DPC_BUFBUSY_REG  = &s_dpc_bufbusy_reg;
    core.DPC_PIPEBUSY_REG = &s_dpc_pipebusy_reg;
    core.DPC_TMEM_REG     = &s_dpc_tmem_reg;

    ultramodern::renderer::ViRegs* vi = ultramodern::renderer::get_vi_regs();
    core.VI_STATUS_REG         = &vi->VI_STATUS_REG;
    core.VI_ORIGIN_REG         = &vi->VI_ORIGIN_REG;
    core.VI_WIDTH_REG          = &vi->VI_WIDTH_REG;
    core.VI_INTR_REG           = &vi->VI_INTR_REG;
    core.VI_V_CURRENT_LINE_REG = &vi->VI_V_CURRENT_LINE_REG;
    core.VI_TIMING_REG         = &vi->VI_TIMING_REG;
    core.VI_V_SYNC_REG         = &vi->VI_V_SYNC_REG;
    core.VI_H_SYNC_REG         = &vi->VI_H_SYNC_REG;
    core.VI_LEAP_REG           = &vi->VI_LEAP_REG;
    core.VI_H_START_REG        = &vi->VI_H_START_REG;
    core.VI_V_START_REG        = &vi->VI_V_START_REG;
    core.VI_V_BURST_REG        = &vi->VI_V_BURST_REG;
    core.VI_X_SCALE_REG        = &vi->VI_X_SCALE_REG;
    core.VI_Y_SCALE_REG        = &vi->VI_Y_SCALE_REG;

    RT64::ApplicationConfiguration appConfig;
    appConfig.useConfigurationFile = false;

    app = std::make_unique<RT64::Application>(core, appConfig);

    const ultramodern::renderer::GraphicsConfig& cur = ultramodern::renderer::get_graphics_config();
    set_application_user_config(app.get(), cur);
    app->userConfig.developerMode = debug;

    switch (cur.api_option) {
        case ultramodern::renderer::GraphicsApi::D3D12:  app->userConfig.graphicsAPI = RT64::UserConfiguration::GraphicsAPI::D3D12;     break;
        case ultramodern::renderer::GraphicsApi::Vulkan: app->userConfig.graphicsAPI = RT64::UserConfiguration::GraphicsAPI::Vulkan;    break;
        case ultramodern::renderer::GraphicsApi::Metal:  app->userConfig.graphicsAPI = RT64::UserConfiguration::GraphicsAPI::Metal;     break;
        default:                                         app->userConfig.graphicsAPI = RT64::UserConfiguration::GraphicsAPI::Automatic; break;
    }

    uint32_t thread_id = 0;
#if defined(_WIN32)
    thread_id = window_handle.thread_id;
#endif
    setup_result = map_setup_result(app->setup(thread_id));
    chosen_api = map_graphics_api(app->chosenGraphicsAPI);
    if (setup_result != ultramodern::renderer::SetupResult::Success) {
        // Name the likely cause — a bare result code is useless in a bug report. The window is already
        // open by now, so a failure here is exactly what leaves the player staring at a black window.
        const char* reason = "unknown error";
        switch (setup_result) {
            case ultramodern::renderer::SetupResult::DynamicLibrariesNotFound:
                reason = "required graphics libraries were not found"; break;
            case ultramodern::renderer::SetupResult::InvalidGraphicsAPI:
                reason = "the configured graphics API is not valid for this platform"; break;
            case ultramodern::renderer::SetupResult::GraphicsAPINotFound:
                reason = "no supported graphics API is available (on Linux this usually means the Vulkan loader, libvulkan, is missing)"; break;
            case ultramodern::renderer::SetupResult::GraphicsDeviceNotFound:
                reason = "no compatible GPU/Vulkan device was found (install or update your GPU's Vulkan driver; verify with vulkaninfo)"; break;
            default: break;
        }
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] RT64 setup failed: %s (result %d)\n", reason, static_cast<int>(setup_result));
        app = nullptr;
        return;
    }

    app->setFullScreen(cur.wm_option == ultramodern::renderer::WindowMode::Fullscreen);

    // Snapshot the render-target settings RT64 actually configured (setup may have downgraded MSAA to
    // None if the device doesn't support the requested sample count — line ~340). These are re-asserted
    // on every runtime config change so MSAA/resolution only change across a restart (see members).
    m_live_aa         = app->userConfig.antialiasing;
    m_live_resolution = app->userConfig.resolution;
    m_live_res_mult   = app->userConfig.resolutionMultiplier;
    m_live_down_mult  = app->userConfig.downsampleMultiplier;

    std::fprintf(stderr, "[beetle-adventure-racing-recomp] RT64 initialized (graphics api %d)\n", static_cast<int>(chosen_api));
}

void RT64Context::send_dl(const OSTask* task) {
    app->state->rsp->reset();
    app->interpreter->loadUCodeGBI(task->t.ucode & 0x3FFFFFF, task->t.ucode_data & 0x3FFFFFF, true);
    app->processDisplayLists(app->core.RDRAM, task->t.data_ptr & 0x3FFFFFF, 0, true);
}

void RT64Context::update_screen() {
    g_bar_vi_ticked.store(true);   // signals main.cpp that the VI thread has ticked (dummy mode seeded)
    // Emulator-style pause (in-game pause menu): the frozen sim submits no new display lists and RDRAM never
    // changes, so RT64's content-change present gate would stall the present timeline and freeze the overlay.
    // Routing the runtime pause into RT64 makes it re-submit the last workload+present every VI tick (see
    // State::updateScreen), keeping the present thread and the overlay render hook alive at ~60fps.
    app->setPaused(ultramodern::is_paused());
    app->updateScreen();
}

void RT64Context::shutdown() {
    if (app != nullptr) {
        app->end();
    }
}

void RT64Context::send_dummy_workload(uint32_t fb_address) {
    // Mirrors recompui's RT64Context: a fill of the whole 320x240 framebuffer through the RDP, so
    // the VI has real content to present before the game submits its first display list.
    app->state->listProcessBegin();
    app->state->rdp->setColorImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, fb_address);
    // G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
    // G_TD_CLAMP | G_TP_PERSP | G_CYC_FILL | G_PM_NPRIMITIVE, then G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP.
    app->state->rdp->setOtherMode(0x382C30, 0);
    app->state->rdp->fillRect(0, 0, 320 << 2, 240 << 2);
    app->state->fullSync();
    app->state->listProcessEnd();
}

bool RT64Context::update_config(const ultramodern::renderer::GraphicsConfig& old_config,
                                const ultramodern::renderer::GraphicsConfig& new_config) {
    if (old_config == new_config) {
        return false;
    }
    if (new_config.wm_option != old_config.wm_option) {
        app->setFullScreen(new_config.wm_option == ultramodern::renderer::WindowMode::Fullscreen);
    }
    set_application_user_config(app.get(), new_config);
    // Defer MSAA + resolution to the next launch (see the m_live_* members): re-assert the live
    // render-target settings so they stay exactly as RT64 created them, and do NOT call the
    // crash-prone runtime updateMultisampling() / live resolution reallocation. Every other setting
    // (refresh rate, aspect ratio, HUD ratio, window mode, HDR) still applies live below.
    app->userConfig.antialiasing         = m_live_aa;
    app->userConfig.resolution           = m_live_resolution;
    app->userConfig.resolutionMultiplier = m_live_res_mult;
    app->userConfig.downsampleMultiplier = m_live_down_mult;
    app->updateUserConfig(true);
    return true;
}

void RT64Context::enable_instant_present() {
    // Despite the name (an ultramodern interface hook called once per session from the gfx thread),
    // BAR uses RT64's **Console** presentation mode — present strictly from the VI origin, at VI time.
    // This is what the console does and it's required for BAR's menu page transitions (the "film-roll"),
    // which PAN the VI origin with osViSwapBuffer across a pre-rendered tall framebuffer without redrawing:
    //   - PresentEarly presents only freshly-RENDERED content, so it drops the VI-origin pan entirely
    //     -> the transition collapses to an instant swap (the original R6 bug).
    //   - SkipBuffering presents the just-rendered framebuffer, so when setup renders the destination
    //     page it flashes that page for one frame BEFORE the pan begins (a visible glitch).
    //   - Console presents exactly the VI-origin framebuffer each frame -> the roll scrolls cleanly with
    //     no pre-pan flash. See docs/R6_FILMROLL_FINDINGS.md.
    // BAR_PRESENT_MODE overrides for testing: "console" (default) | "skip" | "early". BAR_INSTANT_PRESENT
    // is a legacy alias for "early" (min latency, but breaks the VI-pan transitions).
    if (app == nullptr) {
        return;
    }
    using Mode = RT64::EnhancementConfiguration::Presentation::Mode;
    Mode m = Mode::Console;
    if (const char* pm = std::getenv("BAR_PRESENT_MODE")) {
        if      (std::strcmp(pm, "skip")  == 0) m = Mode::SkipBuffering;
        else if (std::strcmp(pm, "early") == 0) m = Mode::PresentEarly;
        else                                    m = Mode::Console;
    } else if (std::getenv("BAR_INSTANT_PRESENT") != nullptr) {
        m = Mode::PresentEarly;
    }
    app->enhancementConfig.presentation.mode = m;
    app->updateEnhancementConfig();
}

uint32_t RT64Context::get_display_framerate() const {
    // Report the real monitor / swap-chain refresh rate so RefreshRate::Display tracks the
    // display (144/165/etc.). This feeds ultramodern's display_refresh_rate (events.cpp), which
    // get_target_framerate() uses as RT64's frame-interpolation target. RT64 populates
    // appWindow->refreshRate during setup and refreshes it on window moves, so reading the cached
    // value is cheap and thread-safe even though this is called every VI tick.
    if (app != nullptr && app->appWindow != nullptr) {
        uint32_t rate = app->appWindow->getRefreshRate();
        if (rate != 0) {
            return rate;
        }
    }
    return 60;   // NTSC fallback until the real rate is known
}

float RT64Context::get_resolution_scale() const {
    if (app != nullptr && app->userConfig.resolution == RT64::UserConfiguration::Resolution::Manual) {
        return static_cast<float>(app->userConfig.resolutionMultiplier);
    }
    return 1.0f;
}

} // namespace

// Factory wired into main.cpp's renderer_callbacks.create_render_context.
std::unique_ptr<ultramodern::renderer::RendererContext>
bar_create_rt64_render_context(uint8_t* rdram, ultramodern::renderer::WindowHandle window_handle, bool developer_mode) {
    // RT64's developer mode, on unconditionally.
    //
    // Every path to RT64's debug UI is gated on it: the F1 key handler, the event filter RT64
    // installs for itself, and State::inspect() at the other end -- which is also where the port's
    // HUD inspector is drawn. A debug menu that only exists in a build made for it is a debug menu
    // nobody has when they need it; the person looking at a misplaced HUD element is running the
    // game they downloaded.
    //
    // Nothing is drawn until F1 is pressed: RT64 creates its inspector on the keystroke and
    // State::inspect() returns immediately while there is none, so the cost of leaving this on is a
    // null check per frame. F2 is unbound in the fork for the same reason -- see
    // lib/rt64/src/hle/rt64_application.cpp.
    (void)developer_mode;
    auto context = std::make_unique<RT64Context>(rdram, window_handle, true);
    if (context->valid()) {
        g_bar_renderer_ready.store(true);
    }
    return context;
}
