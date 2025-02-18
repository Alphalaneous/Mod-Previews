
#include <Geode/Geode.hpp>
#include "ImageCache.hpp"


ImageCache* ImageCache::instance = nullptr;

void ImageCache::addImage(CCImage* image, std::string key){

    if(!image) return;
    m_imageDict.push_back({key, image});

    if (m_imageDict.size() > 20) {
        m_imageDict.erase(m_imageDict.begin());
    }
}

CCImage* ImageCache::getImage(std::string key){

    auto it = std::find_if(m_imageDict.begin(), m_imageDict.end(),
        [&key](const auto& pair) { return pair.first == key; });

    return (it != m_imageDict.end()) ? it->second : nullptr;
}