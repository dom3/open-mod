#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;


struct OpenMod {
    static OpenMod* get() {
        static OpenMod* instance = new OpenMod();
        return instance;
    };

    web::WebTask rate(GJGameLevel* level);
    web::WebTask exists(GJGameLevel* level);
    web::WebTask getAll();

    bool inRatingMenu = false;
    matjson::Value cache;

    int getAmountByLevel(GJGameLevel* level);
    int getAmountByLevel(std::string levelId);
};