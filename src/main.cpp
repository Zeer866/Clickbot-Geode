#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "ClickRecorder.hpp"
#include "ClickPlayer.hpp"
#include "UI.hpp"

using namespace geode::prelude;

class $modify(CBBase, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        auto& c = Clickbot::get();
        if (!c.enabled() || m_isEditor) {
            return GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
        // En reproducción se bloquean los clicks manuales
        if (c.mode == Mode::Play && !c.injecting) return;

        if (c.mode == Mode::Record && !c.injecting) {
            c.clicks.push_back({ c.snap(c.frame), down, button, isPlayer1 });
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }

        void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        auto& c = Clickbot::get();
        if (c.enabled() && !m_isEditor) {
            if (c.mode == Mode::Play) playClicksUntilNow(this);
            c.frame++;
        }
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
}
};

class $modify(CBPlay, PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        auto label = CCLabelBMFont::create("", "bigFont.fnt");
        label->setID("clickbot-badge");
        label->setScale(0.4f);
        label->setOpacity(160);
        label->setAnchorPoint({0.f, 1.f});
        auto win = CCDirector::get()->getWinSize();
        label->setPosition({8.f, win.height - 6.f});
        label->setZOrder(100);
        addChild(label);
        updateBadge();
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        auto& c = Clickbot::get();
        if (!m_isPracticeMode || m_checkpointArray->count() == 0) {
            c.cpFrames.clear();
            if (c.mode == Mode::Record) c.clicks.clear();
            c.rewindTo(0);
        }
    }

    void createCheckpoint() {
        PlayLayer::createCheckpoint();
        auto& c = Clickbot::get();
        if (m_checkpointArray->count() > 0) {
            auto cp = m_checkpointArray->lastObject();
            c.cpFrames[cp] = c.frame;
        }
    }

    void loadFromCheckpoint(CheckpointObject* cp) {
        PlayLayer::loadFromCheckpoint(cp);
        auto& c = Clickbot::get();
        auto it = c.cpFrames.find(cp);
        if (it != c.cpFrames.end()) {
            if (c.mode == Mode::Record) c.truncateFrom(it->second);
            c.rewindTo(it->second);
        }
    }

    void onQuit() {
        Clickbot::get().mode = Mode::Off;
        PlayLayer::onQuit();
    }
};

class $modify(CBPause, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        if (!Clickbot::get().enabled()) return;

        auto mk = [](char const* txt) {
            auto s = ButtonSprite::create(txt);
            s->setScale(0.6f);
            return s;
        };

        auto menu = CCMenu::create();
        menu->setID("clickbot-menu");
        menu->addChild(CCMenuItemExt::createSpriteExtra(mk("REC"),
            [](auto) { setMode(Mode::Record); }));
        menu->addChild(CCMenuItemExt::createSpriteExtra(mk("PLAY"),
            [](auto) { setMode(Mode::Play); }));
        menu->addChild(CCMenuItemExt::createSpriteExtra(mk("OFF"),
            [](auto) { setMode(Mode::Off); }));
        menu->setLayout(RowLayout::create()->setGap(6.f));
        menu->setContentSize({300.f, 30.f});
        menu->updateLayout();

        auto win = CCDirector::get()->getWinSize();
        menu->setPosition({win.width / 2, 40.f});
        addChild(menu);
    }
};
