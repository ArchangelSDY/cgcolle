#include "SimilarityImageGrouper.h"

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cgcolle::encoder;

static cv::Mat calcHist(const cv::Mat &image)
{
    cv::Mat hsvImage;
    cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

    int hBins = 50;
    int sBins = 60;
    int histSize[] = { hBins, sBins };
    float hRanges[] = { 0, 180 };
    float sRanges[] = { 0, 256 };
    const float *ranges[] = { hRanges, sRanges };
    int channels[] = { 0, 1 };

    cv::Mat hist;
    cv::calcHist(&hsvImage, 1, channels, cv::Mat(), hist, 2, histSize,
                 ranges, true, false);
    cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    return hist;
}

std::vector<ImageGroup*> SimilarityImageGrouper::group(const std::vector<std::string>& imagePaths)
{
    typedef std::pair<cv::Mat, cv::Size> HistPair;

    // Calc histograms
    std::vector<HistPair> histograms(imagePaths.size());
    std::transform(imagePaths.begin(), imagePaths.end(), histograms.begin(), [](const std::string &imagePath) -> HistPair {
        cv::Mat image = cv::imread(imagePath);
        cv::Mat hist = calcHist(image);

        return HistPair(hist, image.size());
    });

    // Partition sizes
    std::vector<int> sizeLabels(histograms.size());
    int sizeLabelsCnt = cv::partition(histograms, sizeLabels, [](const HistPair &left, const HistPair &right) {
        const cv::Size &lsize = left.second;
        const cv::Size &rsize = right.second;

        return lsize.width == rsize.width && lsize.height == rsize.height;
    });

    // Paritition histograms
    std::vector<int> histLabels(imagePaths.size());
    int histLabelsCnt = cv::partition(histograms, histLabels, [this](const HistPair &left, const HistPair &right) -> bool {
        return cv::compareHist(left.first, right.first, 0) >= 0.95;
    });

    typedef std::tuple<int, int, int> ImageInfo; // idx, size, hist
    std::vector<std::tuple<int, int, int> > imageInfos;
    for (int i = 0; i < imagePaths.size(); ++i) {
        imageInfos.push_back(std::make_tuple(i, sizeLabels[i], histLabels[i]));
    }

    // Sort by labels
    std::sort(imageInfos.begin(), imageInfos.end(), [](const ImageInfo &l, const ImageInfo &r) -> bool {
        // Compare size
        if (std::get<1>(l) != std::get<1>(r)) {
            return std::get<1>(l) > std::get<1>(r);
        }

        // Compare hist
        return std::get<2>(l) > std::get<2>(r);
    });

    // Build groups
    std::vector<ImageGroup*> groups;
    int curSizeLabel = 0;
    int curHistLabel = 0;
    ImageGroup *group = nullptr;
    for (const ImageInfo &imageInfo : imageInfos) {
        int index, sizeLabel, histLabel;
        std::tie(index, sizeLabel, histLabel) = imageInfo;

        const boost::filesystem::path imagePath(imagePaths[index]);
        const std::string fileName = imagePath.filename().string();
        cgcolle::ImageEntry *entry = new cgcolle::ImageEntry(fileName, imagePath.string());

        bool isGroupChanged = sizeLabel != curSizeLabel || histLabel != curHistLabel;
        if (group == nullptr || isGroupChanged) {
            // Main frame
            group = new ImageGroup();
            groups.push_back(group);

            group->mainFrame = entry;
            curSizeLabel = sizeLabel;
            curHistLabel = histLabel;
        } else {
            // Sub frame
            group->subFrames.push_back(entry);
        }
    }

    return groups;
}
