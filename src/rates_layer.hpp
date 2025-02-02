#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class RatesLayer : public CCLayer, LevelManagerDelegate {
    protected:
        CCMenu* m_loadingScreen = nullptr;
        CCMenu* m_mainScreen = nullptr;

        CCLabelBMFont* m_loadingText = nullptr;

        GJListLayer* m_list = nullptr;
        GJLevelList* m_levelList = nullptr;
        LoadingSpinner* m_loading = nullptr;
        EventListener<web::WebTask> m_listener;

        std::vector<std::string> m_fullSearch;

        const int MAX_SEARCH_RESULTS = 10;
        int m_currentPage = 0;

        virtual bool init() ;
        virtual void onExit();
        virtual void keyBackClicked();

        void back(CCObject*);
        void update_list(matjson::Value data);
        void loadLevelsFinished(CCArray* levels, const char*) override;
        
        GJGameLevel* getLevelById(int id);

        virtual ~RatesLayer();

    public:
        static RatesLayer* create();
        void callback(CCObject* sender);

        void reloadList();
};