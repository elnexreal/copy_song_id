#include <Geode/Geode.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

struct CopySongID : Modify<CopySongID, LevelInfoLayer> {
    CustomSongWidget *songWidget;
    CCLabelBMFont *songIDLabel;

    bool init(GJGameLevel *p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        songWidget = this->m_songWidget;
        songIDLabel = songWidget->m_songIDLabel;

        if (!songWidget->m_isRobtopSong) {
            songIDLabel->setVisible(false);

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

            // strings for labels
            auto songIdStr = std::string("Song ID: ") + std::to_string(songWidget->m_customSongID);
            auto sfxSizeStr = std::string("SFX's: " + std::to_string(songWidget->m_sfx.size()));

            // debug thingys
            // log::debug("{}", songIdStr);
            // log::debug("{}", songWidget->m_songs.size());

            // if multiple songs are in the level add the number of extra songs to the label
            if (songWidget->m_songs.size() >= 2) {
                songIdStr.append(" + ").append(std::to_string(songWidget->m_songs.size() - 1).append(" songs"));
            }

            // wrap the song id string to a button
            auto idBtn = CCMenuItemSpriteExtra::create(CCLabelBMFont::create(songIdStr.c_str(), "bigFont.fnt"),
                                                       this,
                                                       menu_selector(CopySongID::onButtonClick));
            idBtn->setAnchorPoint({0.0, 0.5});
            idBtn->setColor({192, 255, 199});
            idBtn->m_startPosition = CCPoint(0.5, 0.5);
            idBtn->m_scaleMultiplier = 1.05;
            menu->addChild(idBtn);

            // create a sfx counter value
            if (songWidget->m_hasSFX) {
                auto sfxLabel = CCLabelBMFont::create(sfxSizeStr.c_str(), "bigFont.fnt");
                sfxLabel->setAnchorPoint({0.0, 0.5});

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
