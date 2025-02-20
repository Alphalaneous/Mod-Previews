#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ImagePopup : public geode::Popup<> {

protected:
    int m_page;
    int m_size;
    std::string m_url;
    CCSprite* m_currentImage;
    CCLabelBMFont* m_imageCount;

    bool setup() override;
    bool init(int, int, std::string);
public:
    static ImagePopup* create(int, int, std::string);
    void onClose(CCObject*) override;
    void onPrev(CCObject* sender);
    void onNext(CCObject* sender);
    void showImage(int page);


};