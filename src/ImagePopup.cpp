#include "ImagePopup.hpp"
#include "ImageCache.hpp"

bool ImagePopup::init(int page, int size, std::string url){
    bool geodeTheme = Loader::get()->getLoadedMod("geode.loader")->getSettingValue<bool>("enable-geode-theme");

    if (!Popup<>::initAnchored(380.f, 260.f, geodeTheme ? "geode.loader/GE_square01.png" : "GJ_square01.png")) return false;
    setTitle("Preview");

    this->setCloseButtonSpr(
        CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/close.png", .85f,
            (geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green)
        )
    );

    m_page = page;
    m_size = size;
    m_url = url;

    showImage(page);

    return true;
}

bool ImagePopup::setup() {

    CCSprite* prevArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCMenuItemSpriteExtra* prevButton = CCMenuItemSpriteExtra::create(prevArrow, this, menu_selector(ImagePopup::onPrev));
    
    prevButton->setPosition({-20, m_buttonMenu->getContentHeight() / 2});

    CCSprite* nextArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    nextArrow->setFlipX(true);

    CCMenuItemSpriteExtra* nextButton = CCMenuItemSpriteExtra::create(nextArrow, this, menu_selector(ImagePopup::onNext));
    
    nextButton->setPosition({m_buttonMenu->getContentWidth() + 20, m_buttonMenu->getContentHeight() / 2});

    m_buttonMenu->addChild(prevButton);
    m_buttonMenu->addChild(nextButton);

    return true;
}

CCSprite* createSprite(CCImage* img) {
    CCTexture2D* texture = new CCTexture2D();
    CCSprite* spr;
    if (texture->initWithImage(img)){
        spr = CCSprite::createWithTexture(texture);
    }
    texture->release();
    return spr;
}

void ImagePopup::showImage(int page) {

    if (m_currentImage) m_currentImage->removeFromParent();
    std::string previewURL = fmt::format("{}{}.png", m_url, page);

    if (CCImage* image = ImageCache::get()->getImage(fmt::format("id-{}", previewURL))){
        m_currentImage = createSprite(image);

        float maxWidth = 340.f;
        float maxHeight = 210.f;

        CCSize originalSize = m_currentImage->getContentSize();

        float scaleX = maxWidth / originalSize.width;
        float scaleY = maxHeight / originalSize.height;

        float scale = std::min(scaleX, scaleY);
        m_currentImage->setScale(scale);

        m_currentImage->setPosition(m_mainLayer->getContentSize() / 2);
        m_currentImage->setPositionY(m_currentImage->getPositionY() - 10);
        m_mainLayer->addChild(m_currentImage);
    }
}

void ImagePopup::onPrev(CCObject* sender) {
    m_page--;
    if (m_page < 1) m_page = m_size;
    showImage(m_page);
}

void ImagePopup::onNext(CCObject* sender) {
    m_page++;
    if (m_page > m_size) m_page = 1;
    showImage(m_page);
}

void ImagePopup::onClose(cocos2d::CCObject*){
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

ImagePopup* ImagePopup::create(int page, int size, std::string url) {

    auto ret = new ImagePopup();
    if (ret->init(page, size, url)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}