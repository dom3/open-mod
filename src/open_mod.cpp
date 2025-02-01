#include "open_mod.hpp"

web::WebTask OpenMod::rate(GJGameLevel *level)
{
    web::WebRequest req = web::WebRequest();
    auto body = matjson::Value();
    body["userid"] = GJAccountManager::get()->m_accountID;
    body["levelid"] = level->m_levelID.value();
    req.bodyJSON(body);

    auto task = req.post("https://open-mod-api.vercel.app/api/rate");
    return task;
}

web::WebTask OpenMod::exists(GJGameLevel *level)
{
    web::WebRequest req = web::WebRequest();
    auto body = matjson::Value();
    body["userid"] = GJAccountManager::get()->m_accountID;
    body["levelid"] = level->m_levelID.value();
    req.bodyJSON(body);

    auto task = req.post("https://open-mod-api.vercel.app/api/exists/");
    return task;
}

web::WebTask OpenMod::getAll()
{
    web::WebRequest req = web::WebRequest();
    auto task = req.get("https://open-mod-api.vercel.app/api/get/");
    return task;
}

int OpenMod::getAmountByLevel(GJGameLevel *level)
{
    for (auto const& levelData : this->cache) {
        if (levelData.getKey().value() == numToString(level->m_levelID.value())) {
            return levelData.asInt().unwrapOr(0);
        }
    }
    return 0;
}

int OpenMod::getAmountByLevel(std::string levelId)
{
        for (auto const& levelData : this->cache) {
        if (levelData.getKey().value() == numToString(levelId)) {
            return levelData.asInt().unwrapOr(0);
        }
    }
    return 0;
}
