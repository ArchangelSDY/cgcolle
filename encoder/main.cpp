#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>

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
        ("dry-run,n", "dry run")
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

class Workspace
{
public:
    Workspace(const std::string &srcPath)
    {
        boost::filesystem::path workDir(srcPath);
        m_workDir = workDir.append("cgcolle");

        if (!boost::filesystem::exists(m_workDir)) {
            boost::filesystem::create_directory(m_workDir);
        }
    }

    ~Workspace()
    {
        boost::filesystem::remove_all(m_workDir);
    }

    std::string directory() const
    {
        return m_workDir.string();
    }

private:
    boost::filesystem::path m_workDir;
};

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

    if (!boost::filesystem::exists(src)) {
        std::cout << "[ERROR] Source directory does not exist" << std::endl;
        return 1;
    }

    // Step 1: Load image paths
    std::vector<std::string> imagePaths;
    if (!scanImages(src, imagePaths)) {
        std::cout << "[ERROR] Fail to scan source images" << std::endl;
        return 1;
    }

    std::cout << "[Info] Total " << imagePaths.size() << " images" << std::endl;

    // Create workspace
    Workspace workspace(src);

    if (imagePaths.empty()) {
        std::cout << "[WARN] Empty image set" << std::endl;
        return 1;
    }

    std::cout << "[Info] Grouping..." << std::endl;

    // Step 2: Group images
    std::unique_ptr<cgcolle::encoder::ImageGrouper>  grouper(new cgcolle::encoder::NameImageGrouper(pattern));
    std::vector<cgcolle::encoder::ImageGroup *> groups = grouper->group(imagePaths);

    std::cout << "[Info] Groups count " << groups.size() << std::endl;

    if (vm.count("dry-run")) {
        std::cout << "[Info] Dry run. Now exiting." << std::endl;
        return 0;
    }

    std::cout << "[Info] Diffing frames..." << std::endl;

    // Step 3: Generate sub frames
    cgcolle::encoder::ImageDiffGenerator diffGen(workspace.directory());

    boost::asio::io_service diffService;
    boost::thread_group diffThreadPool;
    boost::asio::io_service::work diffWork(diffService);

    uint32_t concurrency = boost::thread::hardware_concurrency();
    std::cout << "[Info] Using " << concurrency << " CPU cores" << std::endl;

    for (uint32_t i = 0; i < concurrency; ++i) {
        diffThreadPool.create_thread(boost::bind(&boost::asio::io_service::run, &diffService));
    }

    for (cgcolle::encoder::ImageGroup *group : groups) {
        diffService.post(boost::bind(&cgcolle::encoder::ImageDiffGenerator::generate, boost::ref(diffGen), group));
    }

    diffService.stop();
    diffThreadPool.join_all();

    // Step 4: Assign id to each frame
    uint32_t id = 0;
    for (cgcolle::encoder::ImageGroup *group : groups) {
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

    // Step 5: Package
    cgcolle::encoder::ImageMux mux(output);
    mux.writeGroups(groups);

    std::cout << "[Info] Done." << std::endl;

	return 0;
}