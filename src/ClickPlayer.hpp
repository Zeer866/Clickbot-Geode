#pragma once
#include "ClickRecorder.hpp"

inline void playClicksUntilNow(GJBaseGameLayer* gl) {
    auto& c = Clickbot::get();
    while (c.idx < c.clicks.size() && c.clicks[c.idx].frame <= c.frame) {
        auto k = c.clicks[c.idx++];
        c.injecting = true;
        gl->handleButton(k.down, k.button, k.p1);
        c.injecting = false;
    }
}
