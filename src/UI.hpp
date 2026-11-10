#pragma once
#include "ClickRecorder.hpp"
#include <Geode/binding/PlayLayer.hpp>

inline void updateBadge() {
    auto pl = PlayLayer::get();
    if (!pl) return;
    auto label = static_cast<CCLabelBMFont*>(pl->getChildByID("clickbot-badge"));
    if (!label) return;
    switch (Clickbot::get().mode) {
        case Mode::Record: label->setString("Clickbot: REC"); break;
        case Mode::Play:   label->setString("Clickbot: PLAY"); break;
        default:           label->setString(""); break;
    }
}

inline void setMode(Mode m) {
    auto& c = Clickbot::get();
    if (m == Mode::Record) c.clicks.clear();
    if (m == Mode::Play && c.clicks.empty()) {
        Notification::create("No hay clicks grabados", NotificationIcon::Warning)->show();
        return;
    }
    c.mode = m;
    updateBadge();
}
