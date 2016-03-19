#pragma once

#include <string>
#include <vector>

#include "ImageGroup.h"

namespace cgcolle
{
    namespace encoder
    {
        class ImageGrouper
        {
        public:
            virtual std::vector<ImageGroup *> group(const std::vector<std::string> &imagePaths) = 0;
        };
    }
}
