#pragma once

#include <string>

#include "ImageGroup.h"

namespace cgcolle
{
    namespace encoder
    {
        class ImageMux
        {
        public:
            ImageMux(const std::string &path);

            void writeGroups(const std::vector<ImageGroup *> &groups);

        private:
            void writeHeader(std::ofstream &output);
            void writeMetaChunk(std::ofstream &output, const std::vector<ImageGroup *> &groups);
            void writeMetaImage(std::ofstream &output, const cgcolle::ImageEntry *entry);
            void writeDataChunk(std::ofstream &output, const std::vector<ImageGroup *> &groups);
            void writeDataImage(std::ofstream &output, const cgcolle::ImageEntry *entry);

            std::string m_path;
        };
    }
}
