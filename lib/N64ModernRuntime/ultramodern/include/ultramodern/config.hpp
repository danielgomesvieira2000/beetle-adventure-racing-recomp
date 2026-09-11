#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>
#include <optional>

#include "json/json.hpp"

namespace ultramodern {
    namespace renderer {
        enum class Resolution {
            Original,      // 1x native internal resolution
            Original2x,    // 2x native
            Auto,          // match the window (integer scale)
            Native3x,      // 3x native
            Native4x,      // 4x native
            Native6x,      // 6x native
            Native8x,      // 8x native
            OptionCount
        };
        enum class WindowMode {
            Windowed,
            Fullscreen,
            OptionCount
        };
        enum class HUDRatioMode {
            Original,
            Clamp16x9,
            Full,
            OptionCount
        };
        enum class GraphicsApi {
            Auto,
            D3D12,
            Vulkan,
            Metal,
            OptionCount
        };
        enum class AspectRatio {
            Original,
            Expand,
            Manual,
            OptionCount
        };
        enum class Antialiasing {
            None,
            MSAA2X,
            MSAA4X,
            MSAA8X,
            OptionCount
        };
        enum class RefreshRate {
            Original,
            Display,
            Manual,
            OptionCount
        };
        enum class HighPrecisionFramebuffer {
            Auto,
            On,
            Off,
            OptionCount
        };
        // BAR seam fix (TODO #2B): N64 VI "divot" filter that fills 1px cracks between polygons.
        // Auto = follow the game's VI divotEnable bit (on for BAR's LAN1 mode).
        enum class DivotFilter {
            Auto,
            On,
            Off,
            OptionCount
        };
        // BAR letterbox control: how the image is fit to a window of a different aspect ratio.
        //   Crop      = keep aspect, fill the window, crop the overflow (no bars).
        //   Stretch   = fill the window, distort to its aspect (no bars).
        //   Pillarbox = keep aspect and pad with black bars. Currently the default; see below.
        // Pairs with ar_option=Expand (widen 3D FOV) so Crop/Stretch show more scene instead of just zooming.
        //
        // Crop is deliberately FIRST even though it is not currently the default. The end goal for this
        // project is that the image always fills the window with no black bars, and the first enumerator
        // is what every fallback path resolves to: nlohmann's enum deserializer returns it for an
        // unrecognised string, RT64's clampEnum() returns it for an out-of-range value, and a
        // default-constructed GraphicsConfig that predates this field would zero to it. Putting Pillarbox
        // first (as this enum used to) meant any one of those paths silently reintroduced the bars.
        // Do not reorder.
        //
        // The DEFAULT is Pillarbox only while plain 4:3 is being established as a correct baseline:
        // it is the one mode that shows the whole picture at its true proportions, so it is what
        // "4:3 works" can be judged against. It goes back to Crop once that baseline is signed off.
        enum class PresentFillMode {
            Crop,
            Stretch,
            Pillarbox,
            OptionCount
        };
        enum class PresentationMode {
            Console,
            SkipBuffering,
            PresentEarly
        };

        // Every member has a default initializer, and that is load-bearing rather than tidiness.
        // RecompFrontend's graphics tab applies settings by default-constructing a GraphicsConfig and
        // assigning only the options it knows about (ui_config_tab_graphics.cpp, apply_graphics_config).
        // It cannot know about divot_option or pfm_option, which this project added — so without these
        // initializers those two fields held indeterminate values every time the player touched the
        // graphics menu, which for pfm_option meant the letterbox could reappear at random. Any field
        // added here must carry a default for the same reason.
        class GraphicsConfig {
        public:
            bool developer_mode = false;
            Resolution res_option = Resolution::Auto;
            WindowMode wm_option = WindowMode::Windowed;
            HUDRatioMode hr_option = HUDRatioMode::Original;
            GraphicsApi api_option = GraphicsApi::Auto;
            AspectRatio ar_option = AspectRatio::Original;   // BASELINE: get plain 4:3 right first
            Antialiasing msaa_option = Antialiasing::MSAA4X;
            RefreshRate rr_option = RefreshRate::Display;
            HighPrecisionFramebuffer hpfb_option = HighPrecisionFramebuffer::Auto;
            int rr_manual_value = 60;
            int ds_option = 1;
            DivotFilter divot_option = DivotFilter::Auto;
            // BASELINE: Pillarbox shows the WHOLE picture at its true proportions, which is what
            // "4:3 working properly" means and what the next step is measured against. Removing the
            // bars is a separate, later change: Crop fills the window but cuts 25% off the top and
            // bottom of a 4:3 image, so it cannot be the baseline.
            PresentFillMode pfm_option = PresentFillMode::Pillarbox;

            virtual ~GraphicsConfig() = default;

            auto operator<=>(const GraphicsConfig& rhs) const = default;
        };

        const GraphicsConfig& get_graphics_config();
        void set_graphics_config(const GraphicsConfig& new_config);

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::Resolution, {
            {ultramodern::renderer::Resolution::Original, "Original"},
            {ultramodern::renderer::Resolution::Original2x, "Original2x"},
            {ultramodern::renderer::Resolution::Auto, "Auto"},
            {ultramodern::renderer::Resolution::Native3x, "Native3x"},
            {ultramodern::renderer::Resolution::Native4x, "Native4x"},
            {ultramodern::renderer::Resolution::Native6x, "Native6x"},
            {ultramodern::renderer::Resolution::Native8x, "Native8x"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::WindowMode, {
            {ultramodern::renderer::WindowMode::Windowed, "Windowed"},
            {ultramodern::renderer::WindowMode::Fullscreen, "Fullscreen"}
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::HUDRatioMode, {
            {ultramodern::renderer::HUDRatioMode::Original, "Original"},
            {ultramodern::renderer::HUDRatioMode::Clamp16x9, "Clamp16x9"},
            {ultramodern::renderer::HUDRatioMode::Full, "Full"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::GraphicsApi, {
            {ultramodern::renderer::GraphicsApi::Auto, "Auto"},
            {ultramodern::renderer::GraphicsApi::D3D12, "D3D12"},
            {ultramodern::renderer::GraphicsApi::Vulkan, "Vulkan"},
            {ultramodern::renderer::GraphicsApi::Metal, "Metal"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::AspectRatio, {
            {ultramodern::renderer::AspectRatio::Original, "Original"},
            {ultramodern::renderer::AspectRatio::Expand, "Expand"},
            {ultramodern::renderer::AspectRatio::Manual, "Manual"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::Antialiasing, {
            {ultramodern::renderer::Antialiasing::None, "None"},
            {ultramodern::renderer::Antialiasing::MSAA2X, "MSAA2X"},
            {ultramodern::renderer::Antialiasing::MSAA4X, "MSAA4X"},
            {ultramodern::renderer::Antialiasing::MSAA8X, "MSAA8X"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::RefreshRate, {
            {ultramodern::renderer::RefreshRate::Original, "Original"},
            {ultramodern::renderer::RefreshRate::Display, "Display"},
            {ultramodern::renderer::RefreshRate::Manual, "Manual"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::HighPrecisionFramebuffer, {
            {ultramodern::renderer::HighPrecisionFramebuffer::Auto, "Auto"},
            {ultramodern::renderer::HighPrecisionFramebuffer::On, "On"},
            {ultramodern::renderer::HighPrecisionFramebuffer::Off, "Off"},
        });

        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::DivotFilter, {
            {ultramodern::renderer::DivotFilter::Auto, "Auto"},
            {ultramodern::renderer::DivotFilter::On, "On"},
            {ultramodern::renderer::DivotFilter::Off, "Off"},
        });

        // Crop first: an unrecognised string resolves to the first entry, and this project must never
        // fall back to a mode that adds black bars. See the enum's own comment.
        NLOHMANN_JSON_SERIALIZE_ENUM(ultramodern::renderer::PresentFillMode, {
            {ultramodern::renderer::PresentFillMode::Crop, "Crop"},
            {ultramodern::renderer::PresentFillMode::Stretch, "Stretch"},
            {ultramodern::renderer::PresentFillMode::Pillarbox, "Pillarbox"},
        });
    }
}

#endif
