#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelSearchLayer.hpp>

#include "rates_layer.hpp"

class $modify(_LevelSearchLayer, LevelSearchLayer) {
	bool init(int p0) {
		if (!LevelSearchLayer::init(p0)) return false;
		auto menu = this->getChildByID("other-filter-menu");

        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_starBtnMod_001.png"),
            this,
			menu_selector(RatesLayer::callback)
        );

        menu->addChild(btn);
        menu->updateLayout();

		
		return true;
	}
};