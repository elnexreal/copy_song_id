#include <Geode/Geode.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

struct CopySongID : Modify<CopySongID, LevelInfoLayer> {
    bool init(GJGameLevel *p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1))
            return false;

        auto songWidget = this->m_songWidget;

        if (!songWidget->m_isRobtopSong) {
            // hide the original label (not deleting it because game crash :sob:)
            songWidget->m_songIDLabel->setVisible(false);

            auto nodeLayout = AxisLayout::create(Axis::Column);
            nodeLayout->setCrossAxisOverflow(false);
            nodeLayout->setAutoScale(false);
            nodeLayout->setCrossAxisAlignment(AxisAlignment::Start);
            nodeLayout->setCrossAxisLineAlignment(AxisAlignment::Start);

            auto node = CCMenu::create();
            node->setPosition({-139.f, -22.f});
            node->setAnchorPoint({0.f, 0.5f});
            node->setContentSize({500.f, 75.f});
            node->setLayout(nodeLayout);
            node->setScale(0.35f);

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
            idBtn->setAnchorPoint({0.f, 0.5f});
            idBtn->setColor({192, 255, 199});

            // create a sfx counter value
            if (songWidget->m_hasSFX) {
                auto sfxLabel = CCLabelBMFont::create(sfxSizeStr.c_str(), "bigFont.fnt");
                sfxLabel->setAnchorPoint({0.f, 0.5f});

                node->addChild(sfxLabel);
            }

            node->addChild(idBtn);

            node->updateLayout();

            songWidget->addChild(node);
        }

        return true;
    }

    void onButtonClick(CCObject *sender) {
        auto songWidget = this->m_songWidget;

        try {
            Notification::create(std::string("ID ") + std::to_string(songWidget->m_customSongID) + std::string(" successfully copied"), CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();
            clipboard::write(std::to_string(songWidget->m_customSongID));
        } catch (const std::exception &e) {
            Notification::create(std::string("Failed to copy ID"), CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
        }
    }
};
