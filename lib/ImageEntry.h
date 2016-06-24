#pragma once

#include <string>

namespace cgcolle
{
    class ImageEntry
    {
    public:
        ImageEntry(const std::string &name, const std::string &path = std::string());

        std::string name;
        std::string path;
        uint32_t mainFrameId;
        uint32_t size;
        uint32_t offset;
        bool isError;
    };
}
