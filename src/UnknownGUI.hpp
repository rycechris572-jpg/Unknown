
#pragma once

#include <Geode/Geode.hpp>
#include <algorithm>

bool unknownBufferEnabled();
void unknownPanicDisable();

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
    cocos2d::CCSprite* m_border = nullptr;
    cocos2d::CCSprite* m_panel = nullptr;

    cocos2d::CCMenu* m_navigationMenu = nullptr;
    cocos2d::CCMenu* m_contentMenu = nullptr;

    cocos2d::CCLabelBMFont* m_title = nullptr;
    cocos2d::CCLabelBMFont* m_description = nullptr;
    cocos2d::CCLabelBMFont* m_status = nullptr;
    cocos2d::CCLabelBMFont* m_arrayList = nullptr;
    cocos2d::CCLabelBMFont* m_notification = nullptr;

    bool m_dragging = false;
    bool m_guiLocked = false;
    bool m_showArrayList = true;

    cocos2d::CCPoint m_dragOffset;

    cocos2d::CCLabelBMFont* makeLabel(
        const char* text,
        float scale = 0.5f
    ) {
        auto label = cocos2d::CCLabelBMFont::create(
            text, "bigFont.fnt"
        );

        if (label) {
            label->setScale(scale);
            label->setColor({235, 235, 240});
        }

        return label;
    }

    cocos2d::CCSprite* makePanel(
        float width,
        float height,
        cocos2d::ccColor3B color,
        GLubyte opacity
    ) {
        auto sprite = cocos2d::CCSprite::create(
            "square02_001.png"
        );

        if (!sprite)
            return nullptr;

        sprite->setColor(color);
        sprite->setOpacity(opacity);

        sprite->setScaleX(
            width / sprite->getContentSize().width
        );

        sprite->setScaleY(
            height / sprite->getContentSize().height
        );

        return sprite;
    }

    cocos2d::CCMenuItemLabel* makeButton(
        const char* text,
        cocos2d::SEL_MenuHandler callback,
        float scale = 0.48f
    ) {
        auto label = makeLabel(text, scale);

        if (!label)
            return nullptr;

        auto button = cocos2d::CCMenuItemLabel::create(
            label, this, callback
        );

        if (button)
            button->setOpacity(255);

        return button;
    }

    void notify(const char* message) {
        if (!m_notification)
            return;

        m_notification->stopAllActions();
        m_notification->setString(message);
        m_notification->setVisible(true);
        m_notification->setOpacity(255);

        m_notification->runAction(
            cocos2d::CCFadeTo::create(1.8f, 0)
        );
    }

    void updateArrayList(float) {
        if (!m_arrayList)
            return;

        if (!m_showArrayList) {
            m_arrayList->setVisible(false);
            return;
        }

        if (unknownBufferEnabled()) {
            m_arrayList->setString(
                "UNKNOWN\n\nINPUT BUFFER  [ON]"
            );
            m_arrayList->setColor({255, 190, 220});
        }
        else {
            m_arrayList->setString(
                "UNKNOWN\n\nINPUT BUFFER  [OFF]"
            );
            m_arrayList->setColor({155, 155, 165});
        }

        m_arrayList->setVisible(true);
    }

    void openTab(Tab tab) {
        if (m_activeTab == tab) {
            closeTab();
            return;
        }

        m_activeTab = tab;

        const char* title = "";
        const char* description = "";

        switch (tab) {
            case Tab::Log:
                title = "DIAGNOSTICS";
                description = "UNKNOWN engine status";
                break;

            case Tab::Stages:
                title = "STAGES";
                description = "Prediction configuration";
                break;

            case Tab::Tools:
                title = "TOOLS";
                description = "Available UNKNOWN controls";
                break;

            case Tab::Settings:
                title = "SETTINGS";
                description = "Interface customization";
                break;

            case Tab::Icons:
                title = "ICONS";
                description = "Equipped icon information";
                break;

            default:
                return;
        }

        m_title->setString(title);
        m_description->setString(description);

        m_panel->setVisible(true);
        m_title->setVisible(true);
        m_description->setVisible(true);
        m_contentMenu->setVisible(true);

        m_panel->setOpacity(0);
        m_title->setOpacity(0);
        m_description->setOpacity(0);

        m_panel->runAction(
            cocos2d::CCFadeTo::create(0.16f, 230)
        );

        m_title->runAction(
            cocos2d::CCFadeTo::create(0.16f, 255)
        );

        m_description->runAction(
            cocos2d::CCFadeTo::create(0.16f, 255)
        );

        refreshContent();
    }

    void closeTab() {
        m_activeTab = Tab::None;

        m_panel->setVisible(false);
        m_title->setVisible(false);
        m_description->setVisible(false);
        m_contentMenu->setVisible(false);
    }

    void refreshContent() {
        if (!m_status)
            return;

        switch (m_activeTab) {
            case Tab::Log:
                m_status->setString(
                    unknownBufferEnabled()
                    ? "Input Buffer: ACTIVE"
                    : "Input Buffer: DISABLED"
                );
                break;

            case Tab::Stages:
                m_status->setString(
                    "Prediction engine controls"
                );
                break;

            case Tab::Tools:
                m_status->setString(
                    "Panic is available below"
                );
                break;

            case Tab::Settings:
                m_status->setString(
                    m_guiLocked
                    ? "GUI Lock: ENABLED"
                    : "GUI Lock: DISABLED"
                );
                break;

            case Tab::Icons:
                m_status->setString(
                    "Equipped icons: information unavailable"
                );
                break;

            default:
                m_status->setString("");
                break;
        }
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

    void onPanic(cocos2d::CCObject*) {
        unknownPanicDisable();

        updateArrayList(0.f);
        refreshContent();
        notify("PANIC  -  INPUT BUFFER DISABLED");
    }

    void onLock(cocos2d::CCObject*) {
        m_guiLocked = !m_guiLocked;

        refreshContent();

        notify(
            m_guiLocked
            ? "GUI LOCK ENABLED"
            : "GUI LOCK DISABLED"
        );
    }

    void onArrayList(cocos2d::CCObject*) {
        m_showArrayList = !m_showArrayList;

        updateArrayList(0.f);

        notify(
            m_showArrayList
            ? "ARRAY LIST ENABLED"
            : "ARRAY LIST HIDDEN"
        );
    }

    void registerWithTouchDispatcher() override {
        cocos2d::CCDirector::sharedDirector()
            ->getTouchDispatcher()
            ->addTargetedDelegate(this, 10, true);
    }

    bool ccTouchBegan(
        cocos2d::CCTouch* touch,
        cocos2d::CCEvent*
    ) override {
        if (m_guiLocked || !m_bar)
            return false;

        auto point = convertToNodeSpace(
            touch->getLocation()
        );

        if (!m_bar->boundingBox().containsPoint(point))
            return false;

        // Only the small empty ends of the bar drag.
        // The tab buttons remain clickable.
        float barCenter = m_bar->getPositionX();
        float distance = std::abs(point.x - barCenter);

        if (distance < 190.f)
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

        auto point = convertToNodeSpace(
            touch->getLocation()
        );

        auto newPosition = point + m_dragOffset;

        auto winSize =
            cocos2d::CCDirector::sharedDirector()->getWinSize();

        newPosition.x = std::clamp(
            newPosition.x, 215.f, winSize.width - 215.f
        );

        newPosition.y = std::clamp(
            newPosition.y, 25.f, winSize.height - 25.f
        );

        auto delta = newPosition - m_bar->getPosition();

        m_bar->setPosition(newPosition);
        m_border->setPosition(
            m_border->getPosition() + delta
        );

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

        m_contentMenu->setPosition(
            m_contentMenu->getPosition() + delta
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
        const float barY = winSize.height - 42.f;
        const float panelY = barY - 115.f;

        // -------------------------
        // NAVIGATION BORDER / GLOW
        // -------------------------

        m_border = makePanel(
            426.f, 46.f,
            {255, 255, 255}, 45
        );

        if (!m_border)
            return false;

        m_border->setPosition({centerX, barY});
        addChild(m_border, 0);

        // -------------------------
        // NAVIGATION BAR
        // -------------------------

        m_bar = makePanel(
            420.f, 40.f,
            {12, 12, 15}, 245
        );

        if (!m_bar)
            return false;

        m_bar->setPosition({centerX, barY});
        addChild(m_bar, 1);

        // -------------------------
        // NAVIGATION BUTTONS
        // -------------------------

        auto log = makeButton(
            "LOG", menu_selector(UnknownGUI::onLog)
        );

        auto stages = makeButton(
            "STAGES", menu_selector(UnknownGUI::onStages)
        );

        auto tools = makeButton(
            "TOOLS", menu_selector(UnknownGUI::onTools)
        );

        auto settings = makeButton(
            "SETTINGS", menu_selector(UnknownGUI::onSettings)
        );

        auto icons = makeButton(
            "ICONS", menu_selector(UnknownGUI::onIcons)
        );

        if (!log || !stages || !tools ||
            !settings || !icons)
            return false;

        m_navigationMenu = cocos2d::CCMenu::create(
            log, stages, tools, settings, icons, nullptr
        );

        if (!m_navigationMenu)
            return false;

        m_navigationMenu->setPosition({0.f, 0.f});

        log->setPosition({centerX - 165.f, barY});
        stages->setPosition({centerX - 80.f, barY});
        tools->setPosition({centerX, barY});
        settings->setPosition({centerX + 90.f, barY});
        icons->setPosition({centerX + 170.f, barY});

        addChild(m_navigationMenu, 5);

        // -------------------------
        // FLOATING PANEL
        // -------------------------

        m_panel = makePanel(
            340.f, 170.f,
            {17, 17, 21}, 230
        );

        if (!m_panel)
            return false;

        m_panel->setPosition({centerX, panelY});
        addChild(m_panel, 1);

        m_title = makeLabel("UNKNOWN", 0.62f);

        if (!m_title)
            return false;

        m_title->setColor({255, 190, 220});
        m_title->setPosition({
            centerX, panelY + 62.f
        });

        addChild(m_title, 3);

        m_description = makeLabel(
            "System interface", 0.40f
        );

        if (!m_description)
            return false;

        m_description->setColor({175, 175, 190});
        m_description->setPosition({
            centerX, panelY + 35.f
        });

        addChild(m_description, 3);

        m_status = makeLabel("", 0.43f);

        if (!m_status)
            return false;

        m_status->setPosition({
            centerX, panelY - 3.f
        });

        addChild(m_status, 3);

        // -------------------------
        // PANEL CONTROLS
        // -------------------------

        auto panic = makeButton(
            "PANIC DISABLE",
            menu_selector(UnknownGUI::onPanic),
            0.43f
        );

        auto lock = makeButton(
            "GUI LOCK",
            menu_selector(UnknownGUI::onLock),
            0.43f
        );

        auto array = makeButton(
            "ARRAY LIST",
            menu_selector(UnknownGUI::onArrayList),
            0.43f
        );

        if (!panic || !lock || !array)
            return false;

        m_contentMenu = cocos2d::CCMenu::create(
            panic, lock, array, nullptr
        );

        if (!m_contentMenu)
            return false;

        m_contentMenu->setPosition({0.f, 0.f});

        panic->setPosition({
            centerX - 105.f, panelY - 55.f
        });

        lock->setPosition({
            centerX, panelY - 55.f
        });

        array->setPosition({
            centerX + 105.f, panelY - 55.f
        });

        addChild(m_contentMenu, 5);

        // -------------------------
        // ARRAY LIST
        // -------------------------

        m_arrayList = makeLabel(
            "UNKNOWN\n\nINPUT BUFFER  [ON]", 0.42f
        );

        if (!m_arrayList)
            return false;

        m_arrayList->setAnchorPoint({1.f, 1.f});
        m_arrayList->setPosition({
            winSize.width - 18.f,
            winSize.height - 24.f
        });

        m_arrayList->setColor({255, 190, 220});
        addChild(m_arrayList, 10);

        // -------------------------
        // NOTIFICATIONS
        // -------------------------

        m_notification = makeLabel("", 0.48f);

        if (!m_notification)
            return false;

        m_notification->setColor({255, 205, 225});
        m_notification->setPosition({
            centerX, 35.f
        });

        addChild(m_notification, 20);

        // -------------------------
        // INITIAL STATE
        // -------------------------

        closeTab();

        m_notification->setVisible(false);

        updateArrayList(0.f);

        schedule(
            schedule_selector(UnknownGUI::updateArrayList),
            0.25f
        );

        notify("WELCOME TO UNKNOWN");

        return true;
    }
};
