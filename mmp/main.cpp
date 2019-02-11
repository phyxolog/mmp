#include "pch.hpp"
#include "mmp.hpp"

namespace fs = std::experimental::filesystem;

int main() {
    fs::path path("D:\\Projects\\mmp\\Debug\\stored.zip");
    mmp::FS *file = new mmp::FS(path, mmp::fs_types::readMode, mmp::fs_types::RandomAccess);

    fs::path out("D:\\Projects\\mmp\\Debug\\stored-packed.mcf");
    mmp::FS *cFile = new mmp::FS(out, mmp::fs_types::writeMode);

    // Initialize detectors
    mmp::RiffWaveDetector riffWaveDetector;
    
    mmp::StreamAnalyzer streamAnalyzer(file, DEFAULT_BUFFER_SIZE);
    mmp::StreamCompressor streamCompressor(file, cFile, DEFAULT_BUFFER_SIZE);

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

    return 0;
}
