#pragma once

#include <string>
#include <vector>

#include "ImageGroup.h"

namespace boost
{
    namespace filesystem
    {
        class path;
    }
}

namespace cgcolle
{
    namespace encoder
    {
        class ImageDiffGenerator
        {
        public:
            ImageDiffGenerator(const std::string &workspace);

            void generate(ImageGroup *group);

        private:
            boost::filesystem::path toWorkspacePath(const std::string &name);

            std::string m_workspace;
        };
    }
}