#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/LazySprite.hpp>

using namespace geode::prelude;

class ImagePopup : public geode::Popup<> {

protected:
    int m_page;
    int m_size;
    std::string m_url;
    LazySprite* m_currentImage;
    CCLabelBMFont* m_imageCount;
    CCScale9Sprite* m_bgLayer;
    std::unordered_map<int, Ref<LazySprite>> m_sprites;

    bool setup() override;
    bool init(int, int, std::string);
public:
    static ImagePopup* create(int, int, std::string);
    void onPrev(CCObject* sender);
    void onNext(CCObject* sender);
    void showImage(int page);
    void onLoad(LazySprite* spr);
};