// The Rumble Pak motor BAR pulses, modelled on the host. Ported from Body Harvest's port
// (src/callbacks.cpp, namespace rumble and drive_rumble), with its constants unchanged, and made
// per-port because BAR has four.
//
// BAR sets rumble strength by pulse density rather than by holding the motor on. A race trace
// (BAR_DBG_PAK=1) shows every motor start as a single write followed by stops -- mostly one on, three
// off; sometimes one in two or three -- so the real motor's inertia turns the pulse train into a buzz
// whose strength is the duty cycle (docs/technical/05-runtime-host.md, "Controller Pak and Rumble
// Pak"). recompinput models a motor that is simply on or off and looks at the flag once per rendered
// frame, which sees a random instant of the train, nearly always "off": the motor was driven and
// nothing was felt, which is what the first playtest reported.
//
// So the port keeps the motor model: the time the motor was on between two frames, divided by the
// time between them, is the duty; low-passed like a small DC motor spinning up (40 ms) and down
// (80 ms); times the Rumble Strength slider; sent to both of the pad's motors. The time constants are
// Body Harvest's, judged by feel -- the Rumble Pak's own are not measured.
//
// BAR_RUMBLE_RAW=1 returns to recompinput's on/off path. BAR_RUMBLE_TRACE=1 logs, once a second per
// active port, the motor writes per second, the mean duty, the peak level and the last value sent.

#include "main/bar_rumble.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace bar::rumble {
namespace {

using clock = std::chrono::steady_clock;

struct Motor {
    std::atomic<bool> on{ false };
    std::atomic<int64_t> on_since_ns{ 0 };   // when the motor last went on
    std::atomic<int64_t> on_total_ns{ 0 };   // on-time accumulated up to that point
    std::atomic<uint32_t> calls{ 0 };
};

std::array<Motor, kPorts> g_motor;

int64_t now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now().time_since_epoch()).count();
}

// Total on-time up to `t`, as seen from the main thread.
int64_t on_time(const Motor& m, int64_t t) {
    int64_t total = m.on_total_ns.load(std::memory_order_relaxed);
    if (m.on.load(std::memory_order_relaxed)) {
        total += std::max<int64_t>(0, t - m.on_since_ns.load(std::memory_order_relaxed));
    }
    return total;
}

} // namespace

bool raw_mode() {
    static const bool raw = [] {
        const char* v = std::getenv("BAR_RUMBLE_RAW");
        return v != nullptr && *v != '\0' && *v != '0';
    }();
    return raw;
}

void motor(int port, bool on) {
    if (port < 0 || port >= kPorts) return;
    Motor& m = g_motor[port];
    m.calls.fetch_add(1, std::memory_order_relaxed);
    const bool was = m.on.load(std::memory_order_relaxed);
    if (on == was) return;
    const int64_t t = now_ns();
    if (on) {
        m.on_since_ns.store(t, std::memory_order_relaxed);
    } else {
        m.on_total_ns.fetch_add(t - m.on_since_ns.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }
    m.on.store(on, std::memory_order_relaxed);
}

void step(double slider, bool silenced, uint16_t out[kPorts], bool send[kPorts]) {
    static bool started = false;
    static int64_t last_t = 0;
    static std::array<int64_t, kPorts> last_on{};
    static std::array<double, kPorts> level{};
    static std::array<int, kPorts> last_sent{ -1, -1, -1, -1 };
    static std::array<int64_t, kPorts> last_send_t{};

    const int64_t t = now_ns();
    if (!started) {
        started = true;
        last_t = t;
        for (int p = 0; p < kPorts; p++) last_on[p] = on_time(g_motor[p], t);
    }
    const int64_t dt = t - last_t;
    for (int p = 0; p < kPorts; p++) {
        out[p] = last_sent[p] > 0 ? uint16_t(last_sent[p]) : 0;
        send[p] = false;
    }
    if (dt <= 0) return;
    last_t = t;

    const double scale = std::clamp(slider, 0.0, 100.0) / 100.0;
    static const bool trace = std::getenv("BAR_RUMBLE_TRACE") != nullptr;

    for (int p = 0; p < kPorts; p++) {
        const int64_t on = on_time(g_motor[p], t);
        const double duty = std::clamp(double(on - last_on[p]) / double(dt), 0.0, 1.0);
        last_on[p] = on;

        // A small DC motor: quick to spin up, slower to run down.
        const double tau = duty > level[p] ? 0.04 : 0.08;
        level[p] += (duty - level[p]) * (1.0 - std::exp(-(dt / 1e9) / tau));

        int strength = silenced ? 0 : int(std::lround(level[p] * scale * 0xFFFF));
        if (strength < 0x0400) strength = 0;   // below what a pad motor turns at

        // Send on a change, and refresh a held level before SDL's duration runs out.
        const bool changed = last_sent[p] < 0 || std::abs(strength - last_sent[p]) >= 0x0800 ||
                             (strength == 0) != (last_sent[p] == 0);
        if (changed || (strength != 0 && t - last_send_t[p] > 100'000'000)) {
            send[p] = true;
            last_sent[p] = strength;
            last_send_t[p] = t;
        }
        out[p] = uint16_t(strength);

        if (trace) {
            static std::array<int64_t, kPorts> window_t{};
            static std::array<double, kPorts> duty_sum{}, level_max{};
            static std::array<int, kPorts> frames{};
            if (window_t[p] == 0) window_t[p] = t;
            duty_sum[p] += duty;
            level_max[p] = std::max(level_max[p], level[p]);
            ++frames[p];
            if (t - window_t[p] >= 1'000'000'000) {
                const uint32_t calls = g_motor[p].calls.exchange(0, std::memory_order_relaxed);
                if (calls != 0 || level_max[p] > 0.0) {
                    std::fprintf(stderr, "[bar-rumble] port %d: %u motor writes/s, duty %.2f mean, level %.2f max, sent 0x%04X\n",
                                 p, calls, duty_sum[p] / frames[p], level_max[p], last_sent[p] < 0 ? 0 : last_sent[p]);
                    std::fflush(stderr);
                }
                window_t[p] = t;
                duty_sum[p] = level_max[p] = 0;
                frames[p] = 0;
            }
        }
    }
}

} // namespace bar::rumble
