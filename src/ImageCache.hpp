#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ImageCache {

protected:
    static ImageCache* instance;
public:

    std::vector<std::pair<std::string, Ref<CCImage>>> m_imageDict;

    void addImage(CCImage* image, std::string key);
    CCImage* getImage(std::string key);

    static ImageCache* get(){

        if (!instance) {
            instance = new ImageCache();
        };
        return instance;
    }
};
