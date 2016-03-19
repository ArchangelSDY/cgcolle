#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "ImageGroup.h"
#include "NameImageGrouper.h"

using namespace cgcolle::encoder;

NameImageGrouper::NameImageGrouper(const std::string &pattern) :
    m_pattern(pattern)
{
}

std::vector<ImageGroup*> NameImageGrouper::group(const std::vector<std::string>& imagePaths)
{
    const boost::regex pattern(m_pattern);
    std::vector<ImageGroup *> groups;

    std::string lastPrefix;
    ImageGroup *lastGroup = nullptr;

    for (auto it = imagePaths.begin(); it != imagePaths.end(); it++) {
        const boost::filesystem::path imagePath(*it);
        const std::string fileName = imagePath.filename().string();

        boost::smatch m;
        if (boost::regex_match(fileName, m, pattern)) {
            std::string prefix(m[1].first, m[1].second);
            cgcolle::ImageEntry *entry = new cgcolle::ImageEntry(fileName, imagePath.string());

            if (lastPrefix != prefix) {
                // New group
                lastPrefix = prefix;
                lastGroup = new ImageGroup();
                lastGroup->mainFrame = entry;
                groups.push_back(lastGroup);
            } else {
                // New entry in last group
                lastGroup->subFrames.push_back(entry);
            }
        } else {
            std::cout << "[WARN] Filename " << fileName << " doesn't match pattern. Will mark it as a separate group" << std::endl;

            ImageGroup *group = new ImageGroup();
            group->mainFrame = new cgcolle::ImageEntry(fileName, imagePath.string());
            groups.push_back(group);
        }
    }

    return groups;
}
