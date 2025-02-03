#include "rates_layer.hpp"

#include "open_mod.hpp"

CCPoint LAYER_SIZE = {358.0f, 220.0f};

/* "Some" code borrowed from https://github.com/geode-sdk/textureldr/ (thx)*/
bool RatesLayer::init()
{
    if (!CCLayer::init())
        return false;

    this->setKeypadEnabled(true);

    OpenMod::get()->inRatingMenu = true;

    this->setID("RateLayer"_spr);

    auto background = createLayerBG();
    background->setID("background");
    this->addChild(background);

    GameManager::get()->fadeInMenuMusic();

    auto window = CCDirector::get()->getWinSize();

    auto menu = CCMenu::create();
    menu->setID("menu"_spr);
    menu->setZOrder(10);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(RatesLayer::back)
    );
    backBtn->setPosition(-window.width / 2 + 25.f, window.height / 2 - 25.f);
    backBtn->setID("back-button");
    menu->addChild(backBtn);

    auto loadingLayout = RowLayout::create();
    loadingLayout->setGap(10);

    m_loadingScreen = CCMenu::create();
    m_loadingScreen->setID("loading"_spr);

    this->m_loading = LoadingSpinner::create(5);
    m_loadingScreen->addChild(m_loading);

    m_loadingText = CCLabelBMFont::create("0%", "chatFont.fnt");

    m_loadingScreen->addChild(
        m_loadingText
    );

    m_loadingScreen->setLayout(loadingLayout);

    m_mainScreen = CCMenu::create();
    m_mainScreen->setPosition({0,0});
    m_mainScreen->setContentSize(window);
    m_mainScreen->setID("main"_spr);

    this->addChild(menu);
    this->addChild(m_loadingScreen);

        auto task = OpenMod::get()->getAll();

        m_listener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                log::info("Rate Data: {}", res->string().unwrapOr("Uh oh!"));

                auto data = res->json().unwrapOr(matjson::Value());
                if (data.get<int>("status_code").unwrapOr(1) == 0) {
                    auto dataBody = data.get<matjson::Value>("data").unwrapOr(matjson::Value());

                    update_list(dataBody);
                }
            } else if (web::WebProgress* p = e->getProgress()) {
                gd::string d = "";
                d.append(numToString(p->uploaded()));
                m_loadingText->setCString(d.append("%").c_str());
            } else if (e->isCancelled()) {
                log::info("Request was cancelled.");
            }
        });

        m_listener.setFilter(task);

    return true;
}

void RatesLayer::onExit()
{
    OpenMod::get()->inRatingMenu = false;
}

void RatesLayer::keyBackClicked()
{
    OpenMod::get()->inRatingMenu = false;
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void RatesLayer::back(CCObject*)
{
    keyBackClicked();
}
void RatesLayer::update_list(matjson::Value data)
{
    // borrowed some stuff from ggdp intergration (continue the loop)
    auto glm = GameLevelManager::sharedState();
    glm->m_levelManagerDelegate = this;

    m_fullSearch.clear();
    
    OpenMod::get()->cache = data;
    matjson::Value sorted_data = data;
    for (auto const& levelData : sorted_data) {
        m_fullSearch.push_back(levelData.getKey().value());
    }
    auto search = std::vector<std::string>(
        m_fullSearch.begin() + (m_currentPage * MAX_SEARCH_RESULTS),
        m_fullSearch.begin() + (
            std::min(
                (int)m_fullSearch.size(),
                (m_currentPage + 1) * MAX_SEARCH_RESULTS
            )
        )
    );
    std::sort(
        search.begin(),
        search.end(),
        [](std::string a, std::string b) {
            auto aRatings = OpenMod::get()->getAmountByLevel(a);
            auto bRatings = OpenMod::get()->getAmountByLevel(b);
            return (aRatings > bRatings);
        }
    );
    auto searchObject = GJSearchObject::create(SearchType::Type19, string::join(search, ","));
    auto storedLevels = glm->getStoredOnlineLevels(searchObject->getKey());

    if (storedLevels) {
        loadLevelsFinished(storedLevels, "");
    } else {
        glm->getOnlineLevels(searchObject);
    }
}

void RatesLayer::loadLevelsFinished(CCArray *levels, const char *)
{
        /* Scroll view */

    if (m_list && (m_list->getParent() == m_mainScreen)) m_mainScreen->removeChild(m_list, true);

    auto window = CCDirector::get()->getWinSize();

    m_list = GJListLayer::create(
        CustomListView::create(levels, BoomListType::Level, LAYER_SIZE.y, LAYER_SIZE.x),
        "Open Mod Ratings",
        { 0, 0, 0, 180 },
        LAYER_SIZE.x, LAYER_SIZE.y, 0.f
    );
    m_list->setZOrder(2);
    //m_list->setContentSize(LAYER_SIZE);
    m_list->setPosition((window/2) - (m_list->getContentSize()/2));
    m_list->setID("list"_spr);
    auto listBg = CCScale9Sprite::create(
        "GJ_square03.png", {0,0,80,80}
    );
    listBg->setContentSize(LAYER_SIZE);
    listBg->setPosition((window / 2));
    listBg->setID("list-bg"_spr);

    m_mainScreen->addChild(m_list);

    this->removeChild(m_loadingScreen);
    this->addChild(m_mainScreen);
}

GJGameLevel *RatesLayer::getLevelById(int id)
{
    // Useless now TODO: REMOVE LATER
    return nullptr;
}

RatesLayer *RatesLayer::create()
{
    auto ret = new RatesLayer;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void RatesLayer::callback(CCObject*)
{
	auto scene = CCScene::create();
	auto layer = RatesLayer::create();

	scene->addChild(layer);

	CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

RatesLayer::~RatesLayer()
{
    this->removeAllChildrenWithCleanup(true);
}
