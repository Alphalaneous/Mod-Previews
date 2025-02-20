#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "alphalaneous.alphas_geode_utils/include/NodeModding.h"
#include "alphalaneous.alphas_geode_utils/include/Utils.h"
#include "ImageCache.hpp"
#include "ImagePopup.hpp"

using namespace geode::prelude;

class $nodeModify(MyModPopup, ModPopup) {

	struct Fields {
		std::map<std::string, EventListener<web::WebTask>> m_listeners;
		std::map<int, Ref<CCSprite>> m_previewSprites;
		std::map<int, Ref<CCMenuItemSpriteExtra>> m_previewButtons;
		Ref<CCNode> m_imagesContainer;
		CCMenu* m_imagesList;
		CCMenu* m_showAllMenu;
		bool m_hasShownImages;
		bool m_isShowingDescription;
		std::string m_url;
	};

	static std::string baseEnumsToString(BaseType type, int size, int color) {
		#define ENUMS_TO_STRING(ty_)    \
			case BaseType::ty_: {       \
				sizeStr = baseEnumToString(static_cast<ty_##BaseSize>(size));\
				colorStr = baseEnumToString(static_cast<ty_##BaseColor>(color));\
			} break
		
			const char* typeStr = baseEnumToString(type);
			const char* sizeStr;
			const char* colorStr;
			switch (type) {
				ENUMS_TO_STRING(Circle);
				ENUMS_TO_STRING(Cross);
				ENUMS_TO_STRING(Account);
				ENUMS_TO_STRING(IconSelect);
				ENUMS_TO_STRING(Leaderboard);
				ENUMS_TO_STRING(Editor);
				ENUMS_TO_STRING(Tab);
				ENUMS_TO_STRING(Category);
			}
			return fmt::format("base{}_{}_{}.png", typeStr, sizeStr, colorStr);
		}

	//as per fod's request, check everything and return to prevent side effects
	bool isSafe() {

		CCNode* githubBtn = getChildByIDRecursive("github");
		if (!githubBtn) return false;

		FLAlertLayer* self = reinterpret_cast<FLAlertLayer*>(this);
		if (!self) return false;
		if (!self->m_mainLayer) return false;

		std::optional<CCNode*> firstNodeOpt = AlphaUtils::Cocos::getChildByClassName(self->m_mainLayer, "cocos2d::CCNode", 0);
		if (!firstNodeOpt.has_value()) return false;

		if (CCString* url = static_cast<CCString*>(githubBtn->getUserObject("url"))) {
			std::string githubURL = url->getCString();
			if (githubURL.empty()) return false;
		}
		else return false;

		//check if sprites exist ig
		if (!CCSprite::createWithSpriteFrameName("edit_addCBtn_001.png")) return false;

		bool geodeTheme = Loader::get()->getLoadedMod("geode.loader")->getSettingValue<bool>("enable-geode-theme");

		CircleBaseColor base = geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green;

		if (base == CircleBaseColor::DarkPurple && !CCSprite::createWithSpriteFrameName("geode.loader/baseCircle_Medium_DarkPurple.png")) return false;
		if (base == CircleBaseColor::Green && !CCSprite::createWithSpriteFrameName("geode.loader/baseCircle_Medium_Green.png")) return false;

		return true;
	}

	void checkIfMain(std::string url, std::function<void(bool)> callback) {
		auto fields = m_fields.self();
		auto req = web::WebRequest();

		fields->m_listeners[url].bind([this, callback](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				callback(res->ok());
			}
		});
		
		auto downloadTask = req.get(url);
		fields->m_listeners[url].setFilter(downloadTask);
	}

	void modify() {
		
		if (!isSafe()) return;

		CCNode* githubBtn = getChildByIDRecursive("github");

		auto fields = m_fields.self();

		fields->m_imagesList = CCMenu::create();
		fields->m_imagesList->setZOrder(1);
		fields->m_imagesList->setContentSize({262, 54});
		fields->m_imagesList->setAnchorPoint({0.5, 0.5});
		fields->m_imagesList->ignoreAnchorPointForPosition(false);

		fields->m_imagesContainer = CCNode::create();
		fields->m_imagesContainer->setID("images-container"_spr);
		fields->m_imagesContainer->setVisible(false);
		fields->m_imagesContainer->setContentSize({267.5, 59});
		fields->m_imagesContainer->setPosition({152.5, 0});
		fields->m_imagesContainer->setAnchorPoint({0, 0});
		fields->m_imagesContainer->addChild(fields->m_imagesList);

		fields->m_showAllMenu = CCMenu::create();
		fields->m_showAllMenu->setContentSize({20, 54});
		fields->m_showAllMenu->setPosition({fields->m_imagesContainer->getContentWidth() - 2.5f, fields->m_imagesContainer->getContentHeight()/2});
		fields->m_showAllMenu->ignoreAnchorPointForPosition(false);
		fields->m_showAllMenu->setAnchorPoint({1, 0.5});
		fields->m_showAllMenu->setZOrder(2);
		fields->m_showAllMenu->setVisible(false);

		bool geodeTheme = Loader::get()->getLoadedMod("geode.loader")->getSettingValue<bool>("enable-geode-theme");

		CCSprite* moreSpr = CircleButtonSprite::createWithSpriteFrameName(
            "edit_addCBtn_001.png", .85f,
            (geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green)
        );

		CCMenuItemSpriteExtra* showAllBtn = CCMenuItemSpriteExtra::create(moreSpr, this, menu_selector(MyModPopup::showPopup));
		showAllBtn->setPosition(fields->m_showAllMenu->getContentSize()/2);
		showAllBtn->setScale(0.4f);
		showAllBtn->setTag(1);
		showAllBtn->m_baseScale = 0.4f;

		fields->m_showAllMenu->addChild(showAllBtn);
		fields->m_imagesContainer->addChild(fields->m_showAllMenu);

		fields->m_imagesList->setPosition(fields->m_imagesContainer->getContentSize()/2);

		CCScale9Sprite* background = CCScale9Sprite::create("square02b_001.png");
		background->setContentSize(fields->m_imagesContainer->getContentSize() / 0.5);
		background->setScale(0.5);
		background->setOpacity(75);
		background->setColor({0, 0, 0});
		background->setPosition(fields->m_imagesContainer->getContentSize()/2);

		fields->m_imagesContainer->addChild(background);

		CCString* url = static_cast<CCString*>(githubBtn->getUserObject("url"));

		std::string githubURL = url->getCString();
		if (githubURL.empty()) return;

		std::vector<std::string> urlParts = utils::string::split(githubURL, "://github.com/");
		if (urlParts.size() < 2) return;

		std::string ending = urlParts.at(urlParts.size() - 1);
		
		std::string modJsonUrul = fmt::format("https://raw.githubusercontent.com/{}/main/mod.json", ending);

		checkIfMain(modJsonUrul, [this, fields, ending] (bool main) {
			std::string mainBranch = "main";
			if (!main) mainBranch = "master";

			fields->m_url = fmt::format("https://raw.githubusercontent.com/{}/{}/previews/preview-", ending, mainBranch);

			for (int i = 1; i <= 10; i++) {
				std::string previewURL = fmt::format("{}{}.png", fields->m_url, i);

				if (CCImage* image = ImageCache::get()->getImage(fmt::format("id-{}", previewURL))){
					CCSprite* sprite = createSprite(image);
					onImageDownloadFinish(i, sprite);
				}
				else {
					auto req = web::WebRequest();

					fields->m_listeners[previewURL].bind([this, i, previewURL](web::WebTask::Event* e){
						if (auto res = e->getValue()){
							if (res->ok()) {
								auto data = res->data();
								std::thread imageThread = std::thread([this, data, i, previewURL](){
									CCImage* image = new CCImage();
			
									if(!image->initWithImageData(const_cast<uint8_t*>(data.data()), data.size())) return;
									queueInMainThread([this, i, image, previewURL] {
										ImageCache::get()->addImage(image, fmt::format("id-{}", previewURL));
										image->release();
										CCSprite* sprite = createSprite(image);
										onImageDownloadFinish(i, sprite);
									});
								});
								imageThread.detach();
							}
						}
					});
					
					auto downloadTask = req.get(previewURL);
					fields->m_listeners[previewURL].setFilter(downloadTask);
				}
			}
		});

		
	}

	void resizeDescription(CCNode* description) {
		description->setContentHeight(192);
		description->setPositionY(67);

		if (MDTextArea* textArea = static_cast<MDTextArea*>(description->getChildByID("textarea"))) {
			textArea->setContentHeight(description->getContentHeight());
			textArea->setAnchorPoint({0.5, 0});
			textArea->setPositionY(0);
			if (CCScale9Sprite* background = textArea->getChildByType<CCScale9Sprite*>(0)) {
				background->setContentHeight(description->getContentHeight() / background->getScale());
				background->setAnchorPoint({0.5, 0});
				background->setPositionY(0);
			}
			if (ScrollLayer* scrollLayer = textArea->getChildByType<ScrollLayer*>(0)) {
				scrollLayer->setContentHeight(description->getContentHeight());
				scrollLayer->scrollToTop();
			}
		}

	}

	void listenForDescription(float dt) {
		auto fields = m_fields.self();

		CCNode* description = getChildByIDRecursive("description-container");

		if (!fields->m_isShowingDescription && description) {
			fields->m_imagesContainer->setVisible(true);
			resizeDescription(description);
			fields->m_isShowingDescription = true;
		}
		if (fields->m_isShowingDescription && !description){
			fields->m_imagesContainer->setVisible(false);
			fields->m_isShowingDescription = false;
		}
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

	void showImages() {
		auto fields = m_fields.self();

		FLAlertLayer* self = reinterpret_cast<FLAlertLayer*>(this);
		std::optional<CCNode*> firstNodeOpt = AlphaUtils::Cocos::getChildByClassName(self->m_mainLayer, "cocos2d::CCNode", 0);
		if (!firstNodeOpt.has_value()) return;
		CCNode* firstNode = firstNodeOpt.value();

		firstNode->addChild(fields->m_imagesContainer);
		schedule(schedule_selector(MyModPopup::listenForDescription));
	}

	void showPopup(CCObject* sender) {
		auto fields = m_fields.self();

		ImagePopup* popup = ImagePopup::create(sender->getTag(), fields->m_previewSprites.size(), fields->m_url);
		popup->show();
	}

	void onImageDownloadFinish(int id, CCSprite* spr){
		auto fields = m_fields.self();

		float scale = 54/spr->getContentHeight();
		spr->setScale(scale);
		CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MyModPopup::showPopup));
		btn->setTag(id);
		btn->ignoreAnchorPointForPosition(true);
		btn->m_scaleMultiplier = 1.1f;

		fields->m_previewSprites[id] = spr;
		fields->m_previewButtons[id] = btn;

		if (!fields->m_hasShownImages) {
			showImages();
			fields->m_hasShownImages = true;
		}
		
		btn->setID(fmt::format("preview-{}", id));

		fields->m_imagesList->removeAllChildren();

		CCMenuItemSpriteExtra* lastButton = nullptr;

		float gap = 2.5;

		float totalWidth = 0;

		for (auto& [k, v] : fields->m_previewButtons) {
			if (lastButton) {
				float pos = lastButton->getPositionX() + lastButton->getContentWidth();
				if (pos + v->getContentWidth() >= 262) {
					fields->m_showAllMenu->setVisible(true);
					break;
				}
				v->setPositionX(pos + gap);
			}
			totalWidth += v->getContentWidth() + gap;
			lastButton = v;
			fields->m_imagesList->addChild(v);
		}
		totalWidth -= gap;
		fields->m_imagesList->setContentWidth(totalWidth);
		if (fields->m_showAllMenu->isVisible()) {
			fields->m_imagesList->setPositionX((fields->m_imagesContainer->getContentWidth() - fields->m_showAllMenu->getContentWidth()) / 2);
		}
	}
};
