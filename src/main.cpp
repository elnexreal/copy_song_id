#include <Geode/Geode.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

struct CopySongID : Modify<CopySongID, LevelInfoLayer> {
    /*
        define things for the functions (probably bad code)
        btw "o_" means original
    */
    CustomSongWidget *songWidget;
    CCLabelBMFont *o_songIDLabel;

    bool init(GJGameLevel *p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        songWidget = this->m_songWidget;
        o_songIDLabel = songWidget->m_songIDLabel;

        if (!songWidget->m_isRobtopSong) {
            o_songIDLabel->setVisible(false);

            auto menuLayout = AxisLayout::create(Axis::Row);
            menuLayout->setCrossAxisOverflow(false);
            menuLayout->setAutoScale(false);
            menuLayout->setAxisAlignment(AxisAlignment::Start);
            menuLayout->setGap(30.0);

            auto menu = CCMenu::create();
            menu->setPosition({-139.0, -32.0});
            menu->setAnchorPoint({0.0, 0.5});
            menu->setContentSize({600.0, 40.0});
            menu->setLayout(menuLayout);
            menu->setScale(0.35f);
            menu->setID("btnMenu"_spr);

            // strings for labels
            auto songIDStr = std::string("Song ID: ") + std::to_string(songWidget->m_customSongID);
            auto sfxSizeStr = std::string("SFXs: " + std::to_string(songWidget->m_sfx.size()));

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
            if (songWidget->m_songs.size() >= 2) {
                auto songExtras = CCLabelBMFont::create(std::string("(+").append(std::to_string(songWidget->m_songs.size() - 1)).append(std::string(")")).c_str(),
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
            menu->addChild(idBtn);

            // create a sfx counter value
            if (songWidget->m_hasSFX) {
                auto sfxLabel = CCLabelBMFont::create(sfxSizeStr.c_str(),
                                                      "bigFont.fnt");
                sfxLabel->setAnchorPoint({0.0, 0.5});
                sfxLabel->setID("sfxLabel"_spr);

                menu->addChild(sfxLabel);
            }
            menu->updateLayout();
            songWidget->addChild(menu);
        }

        return true;
    }

    void onButtonClick(CCObject *sender) {
        auto songWidget = this->m_songWidget;

        try {
            // copy the song ID to the clipboard
            clipboard::write(std::to_string(songWidget->m_customSongID));
            Notification::create(std::string("ID ") + std::to_string(songWidget->m_customSongID) + std::string(" successfully copied"), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
        } catch (const std::exception &e) {
            Notification::create(std::string("Failed to copy ID"), CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
        }
    }
};
