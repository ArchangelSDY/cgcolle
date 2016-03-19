#include "ImageDiffGenerator.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace cgcolle::encoder;

ImageDiffGenerator::ImageDiffGenerator(const std::string &workspace) :
    m_workspace(workspace)
{
}

void ImageDiffGenerator::generate(ImageGroup *group)
{
    const std::string &mainFramePath = group->mainFrame->path;
    cv::Mat mainFrameImg = cv::imread(mainFramePath);
    boost::filesystem::path mainFrameOutputPath = toWorkspacePath(group->mainFrame->name);
    group->mainFrame->path = mainFrameOutputPath.string();
    cv::imwrite(mainFrameOutputPath.string(), mainFrameImg);

    for (auto it = group->subFrames.begin(); it != group->subFrames.end(); it++) {
        const std::string &subFramePath = (*it)->path;
        cv::Mat subFrameImg = cv::imread(subFramePath);

        cv::Mat diffFrameImg;
        cv::bitwise_xor(mainFrameImg, subFrameImg, diffFrameImg);

        boost::filesystem::path diffFrameOutputPath = toWorkspacePath((*it)->name);
        cv::imwrite(diffFrameOutputPath.string(), diffFrameImg);

        (*it)->path = diffFrameOutputPath.string();
    }
}

boost::filesystem::path ImageDiffGenerator::toWorkspacePath(const std::string &name)
{
    std::ostringstream pathStream;
    pathStream << name;
    pathStream << ".png";

    boost::filesystem::path p(m_workspace);
    p.append(pathStream.str());

    return p;
}