#include "ImagePopup.hpp"
#include "ImageCache.hpp"

bool ImagePopup::init(int page, int size, std::string url){
    bool geodeTheme = Loader::get()->getLoadedMod("geode.loader")->getSettingValue<bool>("enable-geode-theme");

    if (!Popup<>::initAnchored(380.f, 250.f, geodeTheme ? "geode.loader/GE_square01.png" : "GJ_square01.png")) return false;
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

    m_imageCount = CCLabelBMFont::create(fmt::format("Image {}/{}", m_page, m_size).c_str(), "goldFont.fnt");

    m_imageCount->setAnchorPoint({1, 1});
    m_imageCount->setScale(0.4f);
    m_imageCount->setPosition({m_mainLayer->getContentWidth() - 8, m_mainLayer->getContentHeight() - 8});
    m_mainLayer->addChild(m_imageCount);

    return true;
}

void ImagePopup::showImage(int page) {
    if (m_currentImage) {
        m_currentImage->removeFromParent();
    }
    std::string previewURL = fmt::format("{}{}.png", m_url, page);

    LazySprite* spr;

    if (LazySprite* sprite = m_sprites[page]) {
        spr = sprite;
        onLoad(spr);
    }
    else {
        spr = LazySprite::create({100, 50});
        m_sprites[page] = spr;
        spr->setLoadCallback([this, spr](Result<> res) {
            if (res.isOk()) {
                onLoad(spr);
            }
        });
        spr->loadFromUrl(previewURL);
    }
    m_currentImage = spr;
    m_mainLayer->addChild(spr);
}

void ImagePopup::onLoad(LazySprite* spr) {
    float maxWidth = 340.f;
    float maxHeight = 210.f;

    CCSize originalSize = spr->getContentSize();

    float scaleX = maxWidth / originalSize.width;
    float scaleY = maxHeight / originalSize.height;

    float scale = std::min(scaleX, scaleY);

    spr->setScale(scale);

    spr->setPosition(m_mainLayer->getContentSize() / 2);
    spr->setPositionY(spr->getPositionY() - 10);

    CCSize imageSize = spr->getScaledContentSize() + CCSize{6, 6};

    m_imageCount->setString(fmt::format("Image {}/{}", m_page, m_size).c_str());
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

ImagePopup* ImagePopup::create(int page, int size, std::string url) {

    auto ret = new ImagePopup();
    if (ret->init(page, size, url)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}