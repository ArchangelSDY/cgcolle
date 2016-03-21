#pragma once

#include "ImageGrouper.h"

namespace cgcolle
{
    namespace encoder
    {
        class SimilarityImageGrouper : public ImageGrouper
        {
        public:
            // SimilarityImageGrouper();

            // Inherited via ImageGrouper
            virtual std::vector<ImageGroup*> group(const std::vector<std::string>& imagePaths) override;

        };
    }
}