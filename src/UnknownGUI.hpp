
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
        None, Log, Stages, Tools, Settings, Icons
    };

    Tab m_activeTab = Tab::None;

    cocos2d::CCSprite* m_bar = nullptr;
    cocos2d::CCSprite* m_panel = nullptr;

    cocos2d::CCMenu* m_navigationMenu = nullptr;

    cocos2d::CCLabelBMFont* m_title = nullptr;
    cocos2d::CCLabelBMFont* m_description = nullptr;

    bool m_dragging = false;
    cocos2d::CCPoint m_dragOffset;

    void openTab(Tab tab) {
        if (m_activeTab == tab) {
            m_activeTab = Tab::None;
            m_panel->setVisible(false);
            m_title->setVisible(false);
            m_description->setVisible(false);
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
                description = "Prediction configuration";
                break;

            case Tab::Tools:
                title = "TOOLS";
                description = "Available assists";
                break;

            case Tab::Settings:
                title = "SETTINGS";
                description = "Customize UNKNOWN";
                break;

            case Tab::Icons:
                title = "ICONS";
                description = "Currently equipped icons";
                break;

            default:
                break;
        }

        m_title->setString(title);
        m_description->setString(description);

        m_panel->setVisible(true);
        m_title->setVisible(true);
        m_description->setVisible(true);

        m_panel->setOpacity(0);
        m_title->setOpacity(0);
        m_description->setOpacity(0);

        m_panel->runAction(
            cocos2d::CCFadeTo::create(0.14f, 225)
        );

        m_title->runAction(
            cocos2d::CCFadeTo::create(0.14f, 255)
        );

        m_description->runAction(
            cocos2d::CCFadeTo::create(0.14f, 255)
        );
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

    void registerWithTouchDispatcher() override {
        cocos2d::CCDirector::sharedDirector()
            ->getTouchDispatcher()
            ->addTargetedDelegate(this, 1, true);
    }

    bool ccTouchBegan(
        cocos2d::CCTouch* touch,
        cocos2d::CCEvent*
    ) override {
        auto point = convertToNodeSpace(touch->getLocation());

        if (!m_bar->boundingBox().containsPoint(point))
            return false;

        m_dragging = true;
        m_dragOffset = m_bar->getPosition() - point;

        return true;
    }

    void ccTouchMoved(
        cocos2d::CCTouch* touch,
        cocos2d::CCEvent*
    ) override {
        if (!m_dragging)
            return;

        auto point = convertToNodeSpace(touch->getLocation());
        auto newPosition = point + m_dragOffset;

        auto winSize =
            cocos2d::CCDirector::sharedDirector()->getWinSize();

        float halfWidth = 210.f;
        float halfHeight = 20.f;

        newPosition.x = std::clamp(
            newPosition.x, halfWidth, winSize.width - halfWidth
        );

        newPosition.y = std::clamp(
            newPosition.y, halfHeight, winSize.height - halfHeight
        );

        auto delta = newPosition - m_bar->getPosition();

        m_bar->setPosition(newPosition);
        m_navigationMenu->setPosition(
            m_navigationMenu->getPosition() + delta
        );

        m_panel->setPosition(
            m_panel->getPosition() + delta
        );

        m_title->setPosition(
            m_title->getPosition() + delta
        );

        m_description->setPosition(
            m_description->getPosition() + delta
        );
    }

    void ccTouchEnded(
        cocos2d::CCTouch*,
        cocos2d::CCEvent*
    ) override {
        m_dragging = false;
    }

    bool init() override {
        if (!CCLayer::init())
            return false;

        auto winSize =
            cocos2d::CCDirector::sharedDirector()->getWinSize();

        const float centerX = winSize.width / 2.f;
        const float barY = winSize.height - 38.f;
        const float panelY = barY - 112.f;

        // Compact navigation bar
        m_bar = cocos2d::CCSprite::create("square02_001.png");

        if (!m_bar)
            return false;

        m_bar->setColor({12, 12, 12});
        m_bar->setOpacity(235);

        m_bar->setScaleX(
            420.f / m_bar->getContentSize().width
        );

        m_bar->setScaleY(
            40.f / m_bar->getContentSize().height
        );

        m_bar->setPosition({centerX, barY});
        addChild(m_bar, 1);

        // Navigation labels
        auto makeLabel = [](const char* text) {
            auto label = cocos2d::CCLabelBMFont::create(
                text, "bigFont.fnt"
            );

            if (label) {
                label->setScale(0.48f);
                label->setColor({240, 240, 240});
            }

            return label;
        };

        auto log = makeLabel("Log");
        auto stages = makeLabel("Stages");
        auto tools = makeLabel("Tools");
        auto settings = makeLabel("Settings");
        auto icons = makeLabel("Icons");

        if (!log || !stages || !tools || !settings || !icons)
            return false;

        auto logButton = cocos2d::CCMenuItemLabel::create(
            log, this, menu_selector(UnknownGUI::onLog)
        );

        auto stagesButton = cocos2d::CCMenuItemLabel::create(
            stages, this, menu_selector(UnknownGUI::onStages)
        );

        auto toolsButton = cocos2d::CCMenuItemLabel::create(
            tools, this, menu_selector(UnknownGUI::onTools)
        );

        auto settingsButton = cocos2d::CCMenuItemLabel::create(
            settings, this, menu_selector(UnknownGUI::onSettings)
        );

        auto iconsButton = cocos2d::CCMenuItemLabel::create(
            icons, this, menu_selector(UnknownGUI::onIcons)
        );

        if (!logButton || !stagesButton || !toolsButton ||
            !settingsButton || !iconsButton)
            return false;

        m_navigationMenu = cocos2d::CCMenu::create(
            logButton,
            stagesButton,
            toolsButton,
            settingsButton,
            iconsButton,
            nullptr
        );

        if (!m_navigationMenu)
            return false;

        m_navigationMenu->setPosition({0.f, 0.f});

        logButton->setPosition({centerX - 165.f, barY});
        stagesButton->setPosition({centerX - 80.f, barY});
        toolsButton->setPosition({centerX, barY});
        settingsButton->setPosition({centerX + 90.f, barY});
        iconsButton->setPosition({centerX + 170.f, barY});

        addChild(m_navigationMenu, 3);

        // Compact floating panel
        m_panel = cocos2d::CCSprite::create("square02_001.png");

        if (!m_panel)
            return false;

        m_panel->setColor({18, 18, 18});
        m_panel->setOpacity(225);

        m_panel->setScaleX(
            330.f / m_panel->getContentSize().width
        );

        m_panel->setScaleY(
            155.f / m_panel->getContentSize().height
        );

        m_panel->setPosition({centerX, panelY});
        addChild(m_panel, 1);

        m_title = cocos2d::CCLabelBMFont::create(
            "", "bigFont.fnt"
        );

        if (!m_title)
            return false;

        m_title->setScale(0.58f);
        m_title->setPosition({
            centerX, panelY + 42.f
        });

        addChild(m_title, 2);

        m_description = cocos2d::CCLabelBMFont::create(
            "", "goldFont.fnt"
        );

        if (!m_description)
            return false;

        m_description->setScale(0.42f);
        m_description->setPosition({
            centerX, panelY + 5.f
        });

        addChild(m_description, 2);

        // Nothing opens automatically.
        m_panel->setVisible(false);
        m_title->setVisible(false);
        m_description->setVisible(false);

        return true;
    }
};
