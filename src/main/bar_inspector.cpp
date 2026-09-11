// The HUD inspector. See bar_inspector.h for what it is for, and docs/HUD-INSPECTOR.md for how it is
// used.

#include "bar_inspector.h"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <imgui.h>

#include <librecomp/game.hpp>

#include "json/json.hpp"

// The hooks the RT64 fork calls. Declared in lib/rt64/src/hle/rt64_bar_hud.h; repeated here rather
// than included, so that this translation unit does not pull RT64's internal headers (and their
// RDP/common types) into the port for four function pointers.
extern "C" {
    extern void (*RT64_PortInspectorHook)();
    extern void (*RT64_BarHudNoteElement)(const char *identity, const char *secondIdentity,
        float minX, float maxX, float minY, float maxY, int givenClass, int isRect);
    extern void (*RT64_BarHudEndFrame)();
    extern int (*RT64_BarHudTagLookup)(const char *identity, const char *secondIdentity, int *outClass);
}

// The game state the SI poll last pushed into the fork (src/main/os_unimpl_stubs.cpp). Read for the
// panel's header line only.
extern "C" unsigned int bar_rt64_get_game_state();

namespace bar::inspector {
namespace {

struct Element {
    std::string identity;
    std::string second_identity;
    float min_x = 0.0f, max_x = 0.0f, min_y = 0.0f, max_y = 0.0f;
    bool is_rect = true;
    int given_class = kCenter;
};

struct Frame {
    uint32_t game_state = 0;
    uint64_t number = 0;
    std::vector<Element> elements;
};

bool g_enabled = false;

// The frame the classifier is filling, and the last one it finished. The panel only ever reads the
// finished one. g_building is touched only by the display-list thread, so it needs no lock of its
// own; the swap at end_frame does.
std::mutex g_mutex;
Frame g_building;
Frame g_published;
uint64_t g_frames = 0;

// What the panel has overridden, by identity, and what hud.json holds. Both are read by the
// classifier on every rectangle of every frame, so they are kept behind one flag: the tables are
// empty in the common case, and this lets the classifier skip the lock entirely rather than taking
// an uncontended one per element per frame. Write inside the lock, read outside.
std::unordered_map<std::string, int> g_overrides;
std::unordered_map<std::string, int> g_tags;          // from hud.json, loaded once at startup
std::atomic<bool> g_any_tags{ false };

// A frame is only interesting while the game is on the screen it was captured from, so the panel can
// hold one still while the game runs on.
bool g_hold = false;
Frame g_held;

const char* class_name(int cls) {
    switch (cls) {
        case kLeft:    return "left";
        case kRight:   return "right";
        case kStretch: return "stretch";
        case kSpill:   return "spill";
        default:       return "center";
    }
}

// The lists hud.json is made of, indexed by Class. Kept as one array so the file format and the
// dropdown cannot drift apart: adding a class means adding it here and in the enum, and nowhere else.
const char* const kClassNames[] = { "center", "left", "right", "stretch", "spill" };
constexpr int kClassCount = int(sizeof(kClassNames) / sizeof(kClassNames[0]));

std::filesystem::path tag_path() {
    return recomp::get_config_path() / "hud.json";
}

void refresh_any_tags() {
    g_any_tags.store(!g_overrides.empty() || !g_tags.empty(), std::memory_order_relaxed);
}

// Reads hud.json at startup. Absent is the normal case and is not an error; malformed is reported,
// because a file that silently does nothing is worse than one that complains.
void load_tags() {
    const std::filesystem::path path = tag_path();
    std::ifstream in(path);
    if (!in) {
        return;
    }

    nlohmann::json doc;
    try {
        in >> doc;
    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] hud.json could not be parsed (%s); "
                             "no tags loaded\n", e.what());
        std::fflush(stderr);
        return;
    }
    if (!doc.is_object()) {
        return;
    }

    for (int cls = 0; cls < kClassCount; ++cls) {
        const auto it = doc.find(kClassNames[cls]);
        if (it == doc.end() || !it->is_array()) {
            continue;
        }
        for (const auto& entry : *it) {
            if (entry.is_string()) {
                g_tags[entry.get<std::string>()] = cls;
            }
        }
    }

    refresh_any_tags();
    if (!g_tags.empty()) {
        // Only when there is something to say. A hud.json with every list empty is the normal
        // state of a fresh settings folder, and a line every launch announcing that nothing
        // was loaded is noise in a log that is being read for other reasons.
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] hud.json: %zu tag(s) loaded from %s\n",
                     g_tags.size(), path.string().c_str());
        std::fflush(stderr);
    }
}

// Writes the overrides into hud.json, keeping any entries already there that the panel has not
// touched. They are read at startup, so a saved override survives a restart -- and an identity worth
// keeping can then be moved into the fork's own classifier.
void save_overrides(std::string& status) {
    nlohmann::json doc;
    const std::filesystem::path path = tag_path();
    {
        std::ifstream in(path);
        if (in) {
            try {
                in >> doc;
            }
            catch (const std::exception&) {
                doc = nlohmann::json::object();
            }
        }
    }
    if (!doc.is_object()) doc = nlohmann::json::object();

    // Rewritten every save rather than only when absent, so that a file left behind by an older
    // build does not keep pointing at the instrument this one replaced. hud.json is edited by hand
    // as often as it is written from the panel, and this line is the only thing in it that says how.
    doc["help"] = "Press F1 in game for the HUD inspector: it lists every 2D element of the current "
                  "frame with its identity (tex:0x... or dl:0x...), outlines one when you hover it, "
                  "and changes its class from a dropdown. Put an identity in one of the lists below "
                  "to tag it by hand. See docs/HUD-INSPECTOR.md.";

    for (const char* list : kClassNames) {
        if (!doc.contains(list) || !doc[list].is_array()) doc[list] = nlohmann::json::array();
    }

    std::unordered_map<std::string, int> overrides;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        overrides = g_overrides;
    }

    for (const auto& [identity, cls] : overrides) {
        // An identity belongs to one list; drop it from the others first, so that changing your mind
        // in the panel does not leave both answers behind.
        for (const char* list : kClassNames) {
            auto& array = doc[list];
            for (auto it = array.begin(); it != array.end();) {
                it = (it->is_string() && it->get<std::string>() == identity) ? array.erase(it)
                                                                            : it + 1;
            }
        }
        doc[class_name(cls)].push_back(identity);
    }

    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream out(path);
    if (!out) {
        status = "could not write " + path.string();
        return;
    }
    out << doc.dump(4) << "\n";
    status = "saved " + std::to_string(overrides.size()) + " override(s) to hud.json";
}

// Where an element lands on the screen, given the class it is being drawn with.
//
// This is the fork's geometry read forwards. The game draws in 320x240; RT64 squeezes that into a 4:3
// box in the middle of the window unless an element carries an extended origin, and an origin pins
// one of its edges to the corresponding edge of the widened frame instead. Stretch spreads it across
// the whole frame. Close enough to point at an element with, which is what it is for -- it is not
// what the renderer computes, and a scissor can still cut it short.
ImVec2 screen_x(float min_x, float max_x, int cls, const ImVec2& origin, const ImVec2& size) {
    const float box = size.y * 4.0f / 3.0f;   // the 4:3 frame, at this height
    float left = origin.x + (size.x - box) * 0.5f;
    float scale = box / 320.0f;
    switch (cls) {
        case kLeft:    left = origin.x; break;
        case kRight:   left = origin.x + size.x - box; break;
        case kStretch: left = origin.x; scale = size.x / 320.0f; break;
        default:       break;   // center, and spill, which does not move anything
    }
    return ImVec2(left + min_x * scale, left + max_x * scale);
}

void outline(const Element& e, int cls, ImU32 colour) {
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 xs = screen_x(e.min_x, e.max_x, cls, vp->Pos, vp->Size);
    const float y_scale = vp->Size.y / 240.0f;
    const float y0 = vp->Pos.y + e.min_y * y_scale;
    const float y1 = vp->Pos.y + e.max_y * y_scale;
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    dl->AddRect(ImVec2(xs.x, y0), ImVec2(xs.y, y1), colour, 0.0f, 0, 2.5f);
}

void draw_panel() {
    Frame frame;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        frame = g_hold ? g_held : g_published;
    }

    ImGui::SetNextWindowSize(ImVec2(720.0f, 520.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Beetle Adventure Racing HUD")) {
        ImGui::End();
        return;
    }

    ImGui::Text("state %u   frame %llu   %zu elements", frame.game_state,
                static_cast<unsigned long long>(frame.number), frame.elements.size());
    ImGui::TextDisabled("F1 closes this menu. RT64's own pause (Debugger tab) freezes the");
    ImGui::TextDisabled("game; Hold keeps this list on one frame while the game runs on.");

    bool hold = g_hold;
    if (ImGui::Checkbox("Hold this frame", &hold)) {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (hold) g_held = g_published;
        g_hold = hold;
    }

    static std::string status;
    ImGui::SameLine();
    if (ImGui::Button("Save to hud.json")) {
        save_overrides(status);
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear overrides")) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_overrides.clear();
        refresh_any_tags();
        status = "overrides cleared";
    }
    if (!status.empty()) ImGui::TextDisabled("%s", status.c_str());

    // A busy frame is a few dozen elements and the one being looked for is usually already known by
    // part of its identity.
    static char filter[64] = "";
    ImGui::SetNextItemWidth(220.0f);
    ImGui::InputTextWithHint("filter", "part of an identity", filter, sizeof(filter));

    ImGui::Separator();

    const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("elements", 5, flags, ImVec2(0.0f, 0.0f))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 28.0f);
        ImGui::TableSetupColumn("identity", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, 86.0f);
        ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed, 86.0f);
        ImGui::TableSetupColumn("class", ImGuiTableColumnFlags_WidthFixed, 118.0f);
        ImGui::TableHeadersRow();

        static int selected_row = -1;
        int hovered_row = -1;
        if (selected_row >= static_cast<int>(frame.elements.size())) selected_row = -1;

        for (size_t i = 0; i < frame.elements.size(); ++i) {
            const Element& e = frame.elements[i];
            if (filter[0] != '\0' &&
                e.identity.find(filter) == std::string::npos &&
                e.second_identity.find(filter) == std::string::npos) {
                continue;
            }
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i));

            // The class this element is actually being drawn with, which is what the outline has to
            // use and what the dropdown has to show.
            int current = e.given_class;
            bool overridden = false;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_overrides.find(e.identity);
                if (it == g_overrides.end()) it = g_overrides.find(e.second_identity);
                if (it != g_overrides.end()) {
                    current = it->second;
                    overridden = true;
                }
            }

            ImGui::TableNextColumn();
            char label[16];
            std::snprintf(label, sizeof(label), "%zu", i);
            const bool is_selected = selected_row == static_cast<int>(i);
            if (ImGui::Selectable(label, is_selected,
                                  ImGuiSelectableFlags_SpanAllColumns |
                                  ImGuiSelectableFlags_AllowItemOverlap)) {
                selected_row = is_selected ? -1 : static_cast<int>(i);
            }
            if (ImGui::IsItemHovered()) hovered_row = static_cast<int>(i);

            ImGui::TableNextColumn();
            ImGui::Text("%s  %s", e.identity.c_str(), e.second_identity.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%.0f..%.0f", e.min_x, e.max_x);
            ImGui::TableNextColumn();
            ImGui::Text("%.0f..%.0f", e.min_y, e.max_y);

            // The class, as a dropdown that starts on whatever the element was given. Choosing
            // another overrides this identity from the next frame; "as classified" takes the
            // override away again.
            ImGui::TableNextColumn();
            const int selected = overridden ? current + 1 : 0;
            const char* preview = overridden ? kClassNames[current] : class_name(e.given_class);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::BeginCombo("##class", preview)) {
                if (ImGui::Selectable("as classified", selected == 0)) {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    g_overrides.erase(e.identity);
                    g_overrides.erase(e.second_identity);
                    refresh_any_tags();
                }
                // Counted from the array rather than written out: a class missing from this list
                // would be invisible rather than broken.
                for (int c = 0; c < kClassCount; ++c) {
                    if (ImGui::Selectable(kClassNames[c], selected == c + 1)) {
                        std::lock_guard<std::mutex> lock(g_mutex);
                        g_overrides[e.identity] = c;
                        refresh_any_tags();
                    }
                }
                ImGui::EndCombo();
            }
            if (overridden) {
                ImGui::SameLine();
                ImGui::TextDisabled("*");
            }

            // Hovering a row draws its box over the game; a click keeps the box up while the
            // dropdown is being used.
            if (hovered_row == static_cast<int>(i)) {
                outline(e, current, IM_COL32(255, 230, 60, 255));
            }
            else if (is_selected) {
                outline(e, current, IM_COL32(80, 200, 255, 220));
            }

            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

// ---- what the RT64 fork calls -----------------------------------------------------------------

void note_element(const char* identity, const char* second_identity,
                  float min_x, float max_x, float min_y, float max_y,
                  int given_class, int is_rect) {
    // A frame of a busy menu is a few dozen elements; the cap is only so that a pathological list
    // cannot grow without bound behind the panel's back.
    if (g_building.elements.size() >= 512) return;
    Element e;
    e.identity = identity != nullptr ? identity : "";
    e.second_identity = second_identity != nullptr ? second_identity : "";
    e.min_x = min_x;
    e.max_x = max_x;
    e.min_y = min_y;
    e.max_y = max_y;
    e.given_class = given_class;
    e.is_rect = is_rect != 0;
    g_building.elements.push_back(std::move(e));
}

void end_frame() {
    // A display list that classified nothing is not a frame worth showing. The panel keeps whatever
    // it last had instead of blinking empty -- which matters most while RT64's debugger has the game
    // paused, since that is when the list is actually being read.
    if (g_building.elements.empty()) {
        return;
    }

    g_building.game_state = bar_rt64_get_game_state();
    g_building.number = ++g_frames;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_published = g_building;
    }
    g_building.elements.clear();
}

// Consulted for every rectangle of every frame, from the display-list thread. The override the panel
// holds wins over hud.json, so a change in the dropdown takes effect on the next frame and can be
// undone without restarting. Either identity may carry the tag.
int tag_lookup(const char* identity, const char* second_identity, int* out_class) {
    if (!g_any_tags.load(std::memory_order_relaxed)) return 0;

    std::lock_guard<std::mutex> lock(g_mutex);
    const auto find = [&](const std::unordered_map<std::string, int>& table, const char* key) {
        if (key == nullptr) return false;
        const auto it = table.find(key);
        if (it == table.end()) return false;
        *out_class = it->second;
        return true;
    };
    if (find(g_overrides, identity) || find(g_overrides, second_identity)) return 1;
    if (find(g_tags, identity) || find(g_tags, second_identity)) return 1;
    return 0;
}

} // namespace

bool enabled() {
    return g_enabled;
}

void init() {
    // On by default. The debug menu is part of the build, not something a player has to have been
    // told to switch on, and the panel costs nothing until RT64's UI is open -- RT64 creates its
    // inspector on the F1 keystroke, and State::inspect() returns immediately while there is none.
    // A debug menu that only exists in a build made for it is a debug menu nobody has when they need
    // it: the person looking at a misplaced HUD element is running the game they downloaded.
    //
    // BAR_INSPECTOR=0 turns the port's half off, for an A/B against the fork's own classification.
    // RT64's half stays on F1 either way.
    const char* env = std::getenv("BAR_INSPECTOR");
    g_enabled = (env == nullptr) || (env[0] != '0');
    if (!g_enabled) {
        std::fprintf(stderr, "[beetle-adventure-racing-recomp] HUD inspector off (BAR_INSPECTOR=0)\n");
        std::fflush(stderr);
    }

    // Tags are loaded either way: they are a saved decision about how the game should look, not a
    // debugging aid, and turning the panel off must not quietly change the picture.
    load_tags();
}

void install() {
    // The tag lookup goes in regardless of the panel, for the same reason.
    RT64_BarHudTagLookup = tag_lookup;

    if (!g_enabled) return;
    RT64_BarHudNoteElement = note_element;
    RT64_BarHudEndFrame = end_frame;
    RT64_PortInspectorHook = draw_panel;
}

} // namespace bar::inspector
