#pragma once

#include <vector>

#include "../lib/ImageEntry.h"

namespace cgcolle
{
    namespace encoder
    {
        class ImageGroup
        {
        public:
            ImageEntry *mainFrame;
            std::vector<ImageEntry *> subFrames;
        };
    }
}