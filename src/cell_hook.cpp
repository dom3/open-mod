#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/LevelCell.hpp>
#include "open_mod.hpp"

std::string removal[] = {
    "length-icon",
    "downloads-icon",
    "likes-icon",
    "length-label",
    "downloads-label",
    "likes-label"
};

class $modify(_LevelCell, LevelCell) {
    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();

        if (OpenMod::get()->inRatingMenu) {
            auto mainLayer = this->getChildByID("main-layer");
            for (auto const& removalId : removal) {
                mainLayer->removeChildByID(removalId);
            }
            auto icon = CCSprite::createWithSpriteFrameName(
                "GJ_starsIcon_001.png"
            );
            icon->setID("icon"_spr);
            icon->setZOrder(5);
            icon->setPosition({59, 14});
            icon->setScale(0.5f);
            
            auto rates = OpenMod::get()->getAmountByLevel(m_level);
            
            auto label = CCLabelBMFont::create(numToString(rates).c_str(), "bigFont.fnt");
            label->setAnchorPoint({0.0f, 0.5f});
            label->setPosition({69, 14});
            label->setScale(0.5f);

            mainLayer->addChild(icon);
            mainLayer->addChild(label);

            mainLayer->updateLayout();
        }
    }
};