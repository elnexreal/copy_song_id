#include <Geode/Geode.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

struct CopySongID : Modify<CopySongID, LevelInfoLayer> {
    /*
        references to fields
        btw "o_" means original
    */
    CustomSongWidget *songWidget;
    CCLabelBMFont *o_songIDLabel;
    CCLabelBMFont *sfxLabel;
    CCMenu *menu;
    std::string sfxSizeStr;

    bool init(GJGameLevel *p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        m_fields->songWidget = this->m_songWidget;
        m_fields->o_songIDLabel = m_fields->songWidget->m_songIDLabel;

        if (!m_fields->songWidget->m_isRobtopSong) {
            m_fields->o_songIDLabel->setVisible(false);

            auto menuLayout = AxisLayout::create(Axis::Row);
            menuLayout->setCrossAxisOverflow(false);
            menuLayout->setAutoScale(false);
            menuLayout->setAxisAlignment(AxisAlignment::Start);
            menuLayout->setGap(30.0);

            m_fields->menu = CCMenu::create();
            m_fields->menu->setPosition({-139.0, -32.0});
            m_fields->menu->setAnchorPoint({0.0, 0.5});
            m_fields->menu->setContentSize({600.0, 40.0});
            m_fields->menu->setLayout(menuLayout);
            m_fields->menu->setScale(0.35f);
            m_fields->menu->setID("btnMenu"_spr);

            // strings for labels
            auto songIDStr = std::string("Song ID: ") + std::to_string(m_fields->songWidget->m_customSongID);
            m_fields->sfxSizeStr = std::string("SFXs: " + std::to_string(m_fields->songWidget->m_sfx.size()));

            // add a node that contains two labels for the songs, one for the id and the other for the extras
            auto btnNode = CCNode::create();
            btnNode->setAnchorPoint({0.0, 0.5});
            btnNode->setLayout(AxisLayout::create());
            btnNode->setID("labelNode"_spr);

            // create the label for the id
            auto songIDLabel = CCLabelBMFont::create(songIDStr.c_str(), "bigFont.fnt");
            songIDLabel->setColor({192, 255, 199});
            songIDLabel->setID("songIDLabel"_spr);

            // add the song ID label to the node
            btnNode->addChild(songIDLabel);

            // if multiple songs are in the level add another label to the node
            if (m_fields->songWidget->m_songs.size() >= 2) {
                auto songExtras = CCLabelBMFont::create(std::string("(+").append(std::to_string(m_fields->songWidget->m_songs.size() - 1)).append(std::string(")")).c_str(),
                                                        "chatFont.fnt");
                songExtras->setScale(1.1);
                songExtras->setID("songExtras"_spr);
                btnNode->addChild(songExtras);
            }

            btnNode->setContentSize(songIDLabel->getScaledContentSize());
            btnNode->updateLayout();

            // wrap the song id label to a button
            auto idBtn = CCMenuItemSpriteExtra::create(
                btnNode,
                this,
                menu_selector(CopySongID::onButtonClick));
            idBtn->setAnchorPoint({0.0, 0.5});
            idBtn->m_scaleMultiplier = 1.05;
            idBtn->setID("songIDBtn"_spr);
            m_fields->menu->addChild(idBtn);

            // create a sfx counter value
            if (m_fields->songWidget->m_hasSFX) {
                m_fields->sfxLabel = CCLabelBMFont::create(m_fields->sfxSizeStr.c_str(),
                                                           "bigFont.fnt");
                m_fields->sfxLabel->setAnchorPoint({0.0, 0.5});
                m_fields->sfxLabel->setID("sfxLabel"_spr);

                m_fields->menu->addChild(m_fields->sfxLabel);
            }
            m_fields->menu->updateLayout();
            m_fields->songWidget->addChild(m_fields->menu);
        }

        return true;
    }

    void onButtonClick(CCObject *sender) {
        auto songID = m_fields->songWidget->m_customSongID;

        try {
            // copy the song ID to the clipboard
            clipboard::write(std::to_string(songID));
            Notification::create(std::string("ID ") + std::to_string(songID) + std::string(" successfully copied"), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
        } catch (const std::exception &e) {
            Notification::create(std::string("Failed to copy ID"), CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
        }
    }

    void levelDownloadFinished(GJGameLevel *level) {
        LevelInfoLayer::levelDownloadFinished(level);

        /* robtop doesn't return sfx count when getting the level info
        so i will check if it has sfxs when you finish the download :)*/

        if (m_fields->songWidget->m_isRobtopSong)
            return;

        if (m_fields->songWidget->m_hasSFX) {
            // if the label doesn't exist (created when init() is called)
            if (m_fields->sfxLabel == nullptr) {
                // fetch the sfx count again
                m_fields->sfxSizeStr = std::string("SFXs: " + std::to_string(m_fields->songWidget->m_sfx.size()));

                // repeat all the steps from the sfx condition
                m_fields->sfxLabel = CCLabelBMFont::create(m_fields->sfxSizeStr.c_str(),
                                                           "bigFont.fnt");
                m_fields->sfxLabel->setAnchorPoint({0.0, 0.5});
                m_fields->sfxLabel->setID("sfxLabel"_spr);

                m_fields->menu->addChild(m_fields->sfxLabel);
                m_fields->menu->updateLayout();
            }
        }
    }
};
