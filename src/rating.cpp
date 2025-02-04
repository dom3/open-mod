#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

#include "open_mod.hpp"

#include "authentication.hpp"

using namespace geode::prelude;

#include <Geode/modify/LevelInfoLayer.hpp>

class $modify(_LevelInfoLayer, LevelInfoLayer) {

    struct Fields {
        EventListener<web::WebTask> m_listener;
        bool loading;
    };

    void setText(const char* text, bool clickable) {
        auto menu = this->getChildByID("back-menu");
        auto label = CCLabelBMFont::create(text, "chatFont.fnt");
        label->setOpacity(clickable ? 100 : 25);
        label->setScale(0.8f);

        auto btn = CCMenuItemSpriteExtra::create(
            label,
            this,
            (clickable ? menu_selector(_LevelInfoLayer::onOpenRate) : menu_selector(_LevelInfoLayer::failedToRate))
        );
        menu->addChild(btn);
        menu->updateLayout();
    }

    void failedToRate(CCObject* sender) {
        FLAlertLayer::create(
            "Failed!",
            "You already rated " + (m_level->m_levelName) + " or it's already been rated by a moderator.",
            "Close"
        )->show();
        return;
    }

    void onOpenRate(CCObject* sender) {
        geode::log::info("Rating {}", m_level->m_levelName);
        if (m_fields->loading) return;
        m_fields->loading = true;

        AuthenticationManager::get()->getAuthenticationToken([&](std::string token) {
            auto task = OpenMod::get()->rate(m_level, token);

            m_fields->m_listener.bind([this, sender] (web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    log::info("Rate Data: {}", res->string().unwrapOr("Uh oh!"));
                    auto data = res->json().unwrap();
                    auto status_code = data.get<int>("status_code").unwrapOr(1);
                    if (status_code == 1) {
                        failedToRate(sender);
                        log::error("Already rated by this user {}.", GJAccountManager::get()->m_username);
                        return;
                    } else {
                        FLAlertLayer::create(
                            "Rated!",
                            "You rated " + (m_level->m_levelName),
                            "Close"
                        )->show();
                        return;
                    }
                } else if (web::WebProgress* p = e->getProgress()) {
                } else if (e->isCancelled()) {
                    log::info("Request was cancelled.");
                }
            });

            m_fields->m_listener.setFilter(task);
        });
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        log::info("rate stars {}", level->m_stars.value());

        if (level->m_stars.value()>0) {
            return true;
        }

        auto task = OpenMod::get()->exists(m_level);

        m_fields->m_listener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                log::info("Rate Data: {}", res->string().unwrapOr("Uh oh!"));
                auto data = res->json().unwrapOr(matjson::Value());
                auto status_code = data.get<int>("status_code").unwrapOr(-1);
                if (status_code == 1) {
                    setText("Rate", true);
                    return;
                } else {
                    log::info("User has already rated this level.");
                    setText("Rate", false);
                    return;
                }
            } else if (web::WebProgress* p = e->getProgress()) {
                //log::info("progress: {}", p->downloadProgress().value_or(0.f));
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
            }
        });
        m_fields->m_listener.setFilter(task);

        return true;
    }
};