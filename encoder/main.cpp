#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "ImageDiffGenerator.h"
#include "ImageMux.h"
#include "NameImageGrouper.h"

namespace po = boost::program_options;

static void dumpGroups(const std::vector<cgcolle::encoder::ImageGroup *> &groups)
{
    for (auto it = groups.begin(); it != groups.end(); it++) {
        cgcolle::encoder::ImageGroup *group = *it;
        std::cout << "image group main entry: " << group->mainFrame->name << std::endl;
        for (auto sit = group->subFrames.begin(); sit != group->subFrames.end(); sit++) {
            std::cout << "sub frame: " << (*sit)->path << " main id " << (*sit)->mainFrameId << std::endl;
        }
    }
}

static bool parseArgs(int argc, char **argv, po::variables_map &vm)
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("src,s", po::value<std::string>(), "source directory")
        ("output,o", po::value<std::string>(), "output file")
        ("pattern,p", po::value<std::string>(), "name pattern")
        ;

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return false;
    }

    if (!vm.count("src")) {
        std::cout << "Missing source directory" << std::endl;
        return false;
    }

    if (!vm.count("output")) {
        std::cout << "Missing output file" << std::endl;
        return false;
    }

    if (!vm.count("pattern")) {
        std::cout << "Missing name pattern" << std::endl;
        return false;
    }

    return true;
}

static bool scanImages(const std::string &src, std::vector<std::string> &imagePaths)
{
    boost::filesystem::path srcPath(src);
    boost::filesystem::directory_iterator iter(srcPath);
    for (; iter != boost::filesystem::end(iter); iter++) {
        std::cout << "[Info] Found image: " << iter->path().string() << std::endl;
        imagePaths.push_back(iter->path().string());
    }

    return true;
}

int main(int argc, char **argv)
{
    po::variables_map vm;
    if (!parseArgs(argc, argv, vm)) {
        return 1;
    }

    std::string src = vm["src"].as<std::string>();
    std::cout << "[Info] Source directory: " << src << std::endl;

    std::string output = vm["output"].as<std::string>();
    std::cout << "[Info] Output file: " << output << std::endl;

    std::string pattern = vm["pattern"].as<std::string>();
    std::cout << "[Info] Name pattern: " << pattern << std::endl;

    // Load image paths
    std::vector<std::string> imagePaths;
    if (!scanImages(src, imagePaths)) {
        std::cout << "[ERROR] Fail to scan source images" << std::endl;
        return 1;
    }

    std::cout << "[Info] Total " << imagePaths.size() << " images" << std::endl;

    // Create working directory
    boost::filesystem::path workDir(src);
    workDir.append("cgcolle");
    if (!boost::filesystem::create_directory(workDir)) {
        std::cout << "[ERROR] Fail to create working directory: " << workDir.string() << std::endl;
        return 1;
    }

    if (imagePaths.empty()) {
        std::cout << "[WARN] Empty image set" << std::endl;
        return 1;
    }

    std::cout << "[Info] Grouping..." << std::endl;

    // Group images
    std::unique_ptr<cgcolle::encoder::ImageGrouper>  grouper(new cgcolle::encoder::NameImageGrouper(pattern));
    std::vector<cgcolle::encoder::ImageGroup *> groups = grouper->group(imagePaths);

    std::cout << "[Info] Groups count " << groups.size() << std::endl;
    std::cout << "[Info] Diffing frames..." << std::endl;

    // Generate sub frames
    cgcolle::encoder::ImageDiffGenerator diffGen(workDir.string());

    uint32_t id = 0;
    for (auto it = groups.begin(); it != groups.end(); it++) {
        cgcolle::encoder::ImageGroup *group = *it;

        diffGen.generate(group);

        uint32_t mainFrameId = id;
        group->mainFrame->mainFrameId = id;
        for (int i = 0; i < group->subFrames.size(); ++i) {
            group->subFrames[i]->mainFrameId = mainFrameId;

            // Count one sub frame
            id++;
        }

        // Count one main frame
        id++;
    }

    std::cout << "[Info] Muxing..." << std::endl;

    // Package
    cgcolle::encoder::ImageMux mux(output);
    mux.writeGroups(groups);

    // Clear working directory
    // TODO: Check error
    boost::filesystem::remove_all(workDir);

    std::cout << "[Info] Done." << std::endl;

	return 0;
}