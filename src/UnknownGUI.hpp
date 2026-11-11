
#pragma once

#include <Geode/Geode.hpp>

class UnknownGUI final : public cocos2d::CCLayer {
public:
    static UnknownGUI* create() {
        auto ret = new UnknownGUI();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:
    enum class Tab {
        None,
        Log,
        Stages,
        Tools,
        Settings,
        Icons
    };

    Tab m_activeTab = Tab::None;

    cocos2d::CCSprite* m_barBackground = nullptr;
    cocos2d::CCSprite* m_panelBackground = nullptr;

    cocos2d::CCLabelBMFont* m_panelTitle = nullptr;
    cocos2d::CCLabelBMFont* m_panelDescription = nullptr;

    void openTab(Tab tab) {
        if (m_activeTab == tab) {
            m_activeTab = Tab::None;
            m_panelBackground->setVisible(false);
            m_panelTitle->setVisible(false);
            m_panelDescription->setVisible(false);
            return;
        }

        m_activeTab = tab;

        const char* title = "";
        const char* description = "";

        switch (tab) {
            case Tab::Log:
                title = "LOG";
                description = "Engine logs and diagnostics";
                break;

            case Tab::Stages:
                title = "STAGES";
                description = "Configure prediction stages";
                break;

            case Tab::Tools:
                title = "TOOLS";
                description = "Manage your available assists";
                break;

            case Tab::Settings:
                title = "SETTINGS";
                description = "Customize your UNKNOWN interface";
                break;

            case Tab::Icons:
                title = "ICONS";
                description = "View your equipped icons";
                break;

            default:
                break;
        }

        m_panelTitle->setString(title);
        m_panelDescription->setString(description);

        m_panelBackground->setVisible(true);
        m_panelTitle->setVisible(true);
        m_panelDescription->setVisible(true);
    }

    void onLog(cocos2d::CCObject*) {
        openTab(Tab::Log);
    }

    void onStages(cocos2d::CCObject*) {
        openTab(Tab::Stages);
    }

    void onTools(cocos2d::CCObject*) {
        openTab(Tab::Tools);
    }

    void onSettings(cocos2d::CCObject*) {
        openTab(Tab::Settings);
    }

    void onIcons(cocos2d::CCObject*) {
        openTab(Tab::Icons);
    }

    bool init() override {
        if (!CCLayer::init())
            return false;

        auto winSize =
            cocos2d::CCDirector::sharedDirector()->getWinSize();

        // Navigation bar background
        m_barBackground =
            cocos2d::CCSprite::create("square02_001.png");

        if (!m_barBackground)
            return false;

        m_barBackground->setColor({15, 15, 15});
        m_barBackground->setOpacity(225);
        m_barBackground->setScaleX(560.f / m_barBackground->getContentSize().width);
        m_barBackground->setScaleY(52.f / m_barBackground->getContentSize().height);
        m_barBackground->setPosition({
            winSize.width / 2.f,
            winSize.height - 75.f
        });

        this->addChild(m_barBackground, 1);

        // Navigation buttons
        auto logLabel =
            cocos2d::CCLabelBMFont::create("Log", "bigFont.fnt");
        auto stagesLabel =
            cocos2d::CCLabelBMFont::create("Stages", "bigFont.fnt");
        auto toolsLabel =
            cocos2d::CCLabelBMFont::create("Tools", "bigFont.fnt");
        auto settingsLabel =
            cocos2d::CCLabelBMFont::create("Settings", "bigFont.fnt");
        auto iconsLabel =
            cocos2d::CCLabelBMFont::create("Icons", "bigFont.fnt");

        if (!logLabel || !stagesLabel || !toolsLabel ||
            !settingsLabel || !iconsLabel)
            return false;

        auto logButton =
            cocos2d::CCMenuItemLabel::create(
                logLabel, this, menu_selector(UnknownGUI::onLog));

        auto stagesButton =
            cocos2d::CCMenuItemLabel::create(
                stagesLabel, this, menu_selector(UnknownGUI::onStages));

        auto toolsButton =
            cocos2d::CCMenuItemLabel::create(
                toolsLabel, this, menu_selector(UnknownGUI::onTools));

        auto settingsButton =
            cocos2d::CCMenuItemLabel::create(
                settingsLabel, this, menu_selector(UnknownGUI::onSettings));

        auto iconsButton =
            cocos2d::CCMenuItemLabel::create(
                iconsLabel, this, menu_selector(UnknownGUI::onIcons));

        if (!logButton || !stagesButton || !toolsButton ||
            !settingsButton || !iconsButton)
            return false;

        auto navigationMenu = cocos2d::CCMenu::create(
            logButton,
            stagesButton,
            toolsButton,
            settingsButton,
            iconsButton,
            nullptr
        );

        if (!navigationMenu)
            return false;

        navigationMenu->setPosition({0.f, 0.f});

        const float centerX = winSize.width / 2.f;
        const float barY = winSize.height - 75.f;

        logButton->setPosition({centerX - 215.f, barY});
        stagesButton->setPosition({centerX - 105.f, barY});
        toolsButton->setPosition({centerX, barY});
        settingsButton->setPosition({centerX + 115.f, barY});
        iconsButton->setPosition({centerX + 225.f, barY});

        this->addChild(navigationMenu, 3);

        // Floating menu background
        m_panelBackground =
            cocos2d::CCSprite::create("square02_001.png");

        if (!m_panelBackground)
            return false;

        m_panelBackground->setColor({18, 18, 18});
        m_panelBackground->setOpacity(225);
        m_panelBackground->setScaleX(490.f / m_panelBackground->getContentSize().width);
        m_panelBackground->setScaleY(245.f / m_panelBackground->getContentSize().height);
        m_panelBackground->setPosition({
            centerX,
            winSize.height / 2.f - 35.f
        });

        this->addChild(m_panelBackground, 1);

        // Menu title
        m_panelTitle =
            cocos2d::CCLabelBMFont::create("", "bigFont.fnt");

        if (!m_panelTitle)
            return false;

        m_panelTitle->setScale(0.75f);
        m_panelTitle->setPosition({
            centerX,
            winSize.height / 2.f + 45.f
        });

        this->addChild(m_panelTitle, 2);

        // Menu description
        m_panelDescription =
            cocos2d::CCLabelBMFont::create("", "goldFont.fnt");

        if (!m_panelDescription)
            return false;

        m_panelDescription->setScale(0.55f);
        m_panelDescription->setPosition({
            centerX,
            winSize.height / 2.f - 5.f
        });

        this->addChild(m_panelDescription, 2);

        // Nothing opens automatically.
        m_panelBackground->setVisible(false);
        m_panelTitle->setVisible(false);
        m_panelDescription->setVisible(false);

        return true;
    }
};
