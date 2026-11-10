#pragma once
#include <Geode/Geode.hpp>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace geode::prelude;

struct Click {
    int frame;
    bool down;
    int button;
    bool p1;
};

enum class Mode { Off, Record, Play };

struct Clickbot {
    Mode mode = Mode::Off;
    int frame = 0;
    size_t idx = 0;
    bool injecting = false;
    std::vector<Click> clicks;
    std::unordered_map<void*, int> cpFrames;

    static Clickbot& get() {
        static Clickbot c;
        return c;
    }

    bool enabled() {
        return Mod::get()->getSettingValue<bool>("enabled");
    }

    // Corrección de timing: redondea el frame al múltiplo más cercano
    int snap(int f) {
        int n = std::max<int>(1, Mod::get()->getSettingValue<int64_t>("snap-frames"));
        return ((f + n / 2) / n) * n;
    }

    void rewindTo(int f) {
        frame = f;
        idx = 0;
        while (idx < clicks.size() && clicks[idx].frame < f) idx++;
    }

    void truncateFrom(int f) {
        clicks.erase(
            std::remove_if(clicks.begin(), clicks.end(),
                [f](Click const& c) { return c.frame >= f; }),
            clicks.end());
    }
};
