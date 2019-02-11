#include "pch.hpp"
#include "mmp.hpp"

namespace fs = std::experimental::filesystem;
namespace po = boost::program_options;

void printLogo(bool = true);
int  printUsage();
bool parseArgs(mmp::CLIOptions&, int, char *[]);
bool isAffirmativeAnswer();

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return printUsage();
    }

    // Get current path
    const fs::path CurrentPath = fs::current_path();

    // `command` always the first argument
    std::string Command = argv[1];
    std::transform(Command.begin(), Command.end(), Command.begin(), ::tolower);

    // Init cli options
    mmp::CLIOptions CLIOptions;
    CLIOptions.command = Command;
    CLIOptions.bufferSize = BUFFER_SIZE;
    CLIOptions.verbose = true;
    CLIOptions.enableRiffWaveAnalyze = true;
    CLIOptions.takCompLevel = 9;

    // Input file always the last argument
    CLIOptions.inFile = argv[argc - 1];

    if (!parseArgs(CLIOptions, argc, argv)) {
        return printUsage();
    }

    printLogo();

    uintmax_t FileSize = fs::file_size(CLIOptions.inFile);

    if (CLIOptions.inFile.empty()
        || !fs::exists(CLIOptions.inFile)) {
        std::cout << "[!] No such input file!" << std::endl;
        return 1;
    }

    if (FileSize == 0) {
        std::cout << "[!] Input file was empty {fs::file_size == 0}!" << std::endl;
        return 1;
    }

    // Buffer size must be greater than 0
    if (CLIOptions.bufferSize <= 0) {
        CLIOptions.bufferSize = BUFFER_SIZE;
    }

    if (CLIOptions.bufferSize > FileSize) {
        CLIOptions.bufferSize = static_cast<unsigned int>(FileSize);
    }

    if (CLIOptions.command == COMMAND_EXTRACT
        && CLIOptions.outDir.empty()) {
        CLIOptions.outDir =
            CurrentPath / mmp::Utils::generateUniqueFolderName(CLIOptions.inFile.string(), "extract_data");

        if (!fs::exists(CLIOptions.outDir)) {
            fs::create_directory(CLIOptions.outDir);
        }
    }

    std::cout
        << "-> Buffer size: "
        << mmp::Utils::humanizeSize(CLIOptions.bufferSize)
        << std::endl;

    // Init all boost format templates
    boost::format FoundStreamFormat("--> Found %s @ 0x%016X (%s)");
    boost::format ExtractStreamFormat("%016X-%016X.%s");
    boost::format ExtractPercentFormat("\r-> %i%% completed (%i / %i streams processed)");
    boost::format StatSizeFormat("-> Size of found media streams: %s (%i bytes)");

    mmp::detectorCallbackHandle callbackFn = [&](const mmp::Stream &stream) {
        std::cout
            << FoundStreamFormat
            % stream.getStringType()
            % stream.getOffset()
            % mmp::Utils::humanizeSize(stream.getSize())
            << std::endl;
    };

    std::cout << "-> Scanning..." << std::endl;
    if (CLIOptions.verbose) std::cout << std::endl;
    mmp::FS *file = new mmp::FS(CLIOptions.inFile, mmp::fs_types::readMode, mmp::fs_types::RandomAccess);

    // Initialize detectors
    mmp::RiffWaveDetector riffWaveDetector;
    riffWaveDetector.setCallback(CLIOptions.verbose ? (void*)&callbackFn : nullptr);

    mmp::StreamAnalyzer streamAnalyzer(file, BUFFER_SIZE);
    if (CLIOptions.enableRiffWaveAnalyze) {
        streamAnalyzer.addStreamDetector(&riffWaveDetector);
    }

    try {
        streamAnalyzer.execute();
    }
    catch (const std::exception& err) {
        std::cerr << "[!] " << err.what() << std::endl;
        return 1;
    }

    file->close();
    return 0;

    /*fs::path path("D:\\Projects\\mmp\\Debug\\stored.zip");
    mmp::FS *file = new mmp::FS(path, mmp::fs_types::readMode, mmp::fs_types::RandomAccess);

    fs::path out("D:\\Projects\\mmp\\Debug\\stored-packed.mcf");
    mmp::FS *cFile = new mmp::FS(out, mmp::fs_types::writeMode);

    // Initialize detectors
    mmp::RiffWaveDetector riffWaveDetector;
    
    mmp::StreamAnalyzer streamAnalyzer(file, BUFFER_SIZE);
    mmp::StreamCompressor streamCompressor(file, cFile, BUFFER_SIZE);

    streamAnalyzer.addStreamDetector(&riffWaveDetector);

    try {
        streamAnalyzer.execute();
        streamCompressor.execute(streamAnalyzer.getStreamList());
    } catch (const std::exception& err) {
        std::cerr << "[!] " << err.what() << std::endl;
        return 1;
    }

    file->close();
    cFile->close();

    return 0;*/
}

void printLogo(bool withEndl) {
    std::cout << logo;
    if (withEndl) {
        std::cout << std::endl;
    }
}

int printUsage() {
    printLogo(false);
    std::cout << usageMessage << std::endl;
    return 0;
}

bool parseArgs(mmp::CLIOptions &Options, int argc, char *argv[]) {
    po::options_description desc("");
    desc.add_options()
        ("help,h", "Show help")
        ("verbose,v", po::value<bool>())
        ("wav,w", po::value<bool>())
        ("out,o", po::value<std::string>())
        ("outdir,d", po::value<std::string>())
        ("wavpack,wp", po::value<unsigned short>())
        ("tak,t", po::value<unsigned short>())
        ("bufsize,b", po::value<std::string>());

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv)
        .options(desc)
        .allow_unregistered()
        .run();
    po::store(parsed, vm);

    if (vm.find("out") != vm.end()) {
        Options.outFile = vm["out"].as<std::string>();
    }

    if (vm.find("outdir") != vm.end()) {
        Options.outDir = vm["outdir"].as<std::string>();
    }

    if (vm.find("bufsize") != vm.end()) {
        Options.bufferSize =
            static_cast<unsigned int>(mmp::Utils::memToll(vm["bufsize"].as<std::string>()));
    }

    if (vm.find("wav") != vm.end()) {
        Options.enableRiffWaveAnalyze = vm["wav"].as<bool>();
    }

    if (vm.find("verbose") != vm.end()) {
        Options.verbose = vm["verbose"].as<bool>();
    }

    if (vm.find("tak") != vm.end()) {
        Options.takCompLevel = vm["tak"].as<unsigned short>();
    }

    return vm.find("help") == vm.end();
}

bool isAffirmativeAnswer() {
    std::string ch;
    std::cin >> ch;
    std::transform(ch.begin(), ch.end(), ch.begin(), ::tolower);
    return ch == "y" || ch == "yes" || ch == "yep";
}