#pragma once

#include "ImageGrouper.h"

namespace cgcolle
{
    namespace encoder
    {
        class NameImageGrouper : public ImageGrouper
        {
        public:
            NameImageGrouper(const std::string &pattern);

            // Inherited via ImageGrouper
            virtual std::vector<ImageGroup*> group(const std::vector<std::string>& imagePaths) override;

        private:
            std::string m_pattern;
        };
    }
}
