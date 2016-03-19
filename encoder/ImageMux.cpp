#include "ImageMux.h"

#include <fstream>
#include <boost/filesystem.hpp>

using namespace cgcolle::encoder;

ImageMux::ImageMux(const std::string &path) :
    m_path(path)
{
}

void ImageMux::writeGroups(const std::vector<ImageGroup*> &groups)
{
    std::ofstream output(m_path, std::ofstream::binary);

    writeHeader(output);

    writeMetaChunk(output, groups);

    writeDataChunk(output, groups);

    output.close();
}

void ImageMux::writeHeader(std::ofstream &output)
{
    // Archive header
    static const char RIFF[4] = { 'R', 'I', 'F', 'F' };
    output.write(RIFF, 4);

    uint32_t arcSize = 0;
    output.write(reinterpret_cast<const char *>(&arcSize), sizeof(arcSize));

    static const char CGC[4] = { 'C','G','C', 0 };
    output.write(CGC, 4);
}

void ImageMux::writeMetaChunk(std::ofstream &output, const std::vector<ImageGroup*> &groups)
{
    // Meta chunk
    static const char META[4] = { 'M', 'E', 'T', 'A' };
    output.write(META, 4);

    std::streamoff metaSizePos = output.tellp();

    uint32_t metaSize = 0;
    output.write(reinterpret_cast<const char *>(&metaSize), sizeof(metaSize));

    uint32_t imageCount = 0;
    output.write(reinterpret_cast<const char *>(&imageCount), sizeof(imageCount));

    for (auto it = groups.begin(); it != groups.end(); it++) {
        const ImageGroup *group = *it;

        writeMetaImage(output, group->mainFrame);

        for (auto sit = group->subFrames.begin(); sit != group->subFrames.end(); sit++) {
            const cgcolle::ImageEntry *subFrame = *sit;
            writeMetaImage(output, subFrame);
        }

        imageCount += static_cast<uint32_t>(1 + group->subFrames.size());
    }

    std::streamoff metaEndPos = output.tellp();
    metaSize = static_cast<uint32_t>(metaEndPos - metaSizePos - sizeof(metaSize));

    // TODO: check meta size
    // TODO: check image count size

    // Padding
    if (metaSize % 2 != 0) {
        static const char pad = 0;
        output.write(&pad, 1);
        metaSize += 1;
        metaEndPos = output.tellp();
    }

    output.seekp(metaSizePos);
    output.write(reinterpret_cast<const char *>(&metaSize), sizeof(metaSize));
    output.write(reinterpret_cast<const char *>(&imageCount), sizeof(imageCount));

    output.seekp(metaEndPos);
}

void ImageMux::writeMetaImage(std::ofstream &output, const cgcolle::ImageEntry *image)
{
    // TODO: check name size;
    uint32_t nameSize = static_cast<uint32_t>(image->name.size());
    output.write(reinterpret_cast<const char *>(&nameSize), sizeof(nameSize));
    output.write(image->name.c_str(), image->name.size());

    output.write(reinterpret_cast<const char *>(&image->mainFrameId), sizeof(image->mainFrameId));

    // TODO: check file size
    uint32_t imageSize = static_cast<uint32_t>(boost::filesystem::file_size(image->path));
    output.write(reinterpret_cast<const char *>(&imageSize), sizeof(imageSize));
}

void ImageMux::writeDataChunk(std::ofstream &output, const std::vector<ImageGroup *> &groups)
{
    static const char DATA[4] = { 'D', 'A', 'T', 'A' };
    output.write(DATA, 4);

    std::streamoff dataSizePos = output.tellp();

    uint32_t dataSize = 0;
    output.write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));

    for (auto it = groups.begin(); it != groups.end(); it++) {
        const ImageGroup *group = *it;

        writeDataImage(output, group->mainFrame);

        for (auto sit = group->subFrames.begin(); sit != group->subFrames.end(); sit++) {
            const cgcolle::ImageEntry *subFrame = *sit;
            writeDataImage(output, subFrame);
        }
    }

    std::streamoff dataEndPos = output.tellp();

    // TODO: check data size
    dataSize = static_cast<uint32_t>(dataEndPos - dataSizePos - sizeof(dataSize));

    // Padding
    if (dataSize % 2 != 0) {
        static const char pad = 0;
        output.write(&pad, 1);
        dataSize += 1;
    }

    output.seekp(dataSizePos);
    output.write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
}

void ImageMux::writeDataImage(std::ofstream &output, const cgcolle::ImageEntry *image)
{
    std::fstream imageFile;
    imageFile.open(image->path, std::fstream::in | std::fstream::binary);

    output << imageFile.rdbuf();

    imageFile.close();
}