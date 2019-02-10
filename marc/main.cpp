#include "pch.h"
#include <experimental/filesystem>
#include <list>

#include "StreamAnalyzer.hpp"
#include "RiffWaveDetector.hpp"

#include "StreamCompressor.hpp"

namespace fs = std::experimental::filesystem;

#define DEFAULT_BUFFER_SIZE 1 * 1024 * 1024

int main() {
    fs::path path("D:\\Projects\\marc\\Debug\\stored.zip");
    marc::FS *fsPtr = new marc::FS(path, marc::fs_types::readMode, marc::fs_types::RandomAccess);

    // Initialize detectors
    marc::RiffWaveDetector riffWaveDetector;
    
    marc::StreamAnalyzer streamAnalyzer(fsPtr, DEFAULT_BUFFER_SIZE);
    marc::StreamCompressor streamCompressor(fsPtr, DEFAULT_BUFFER_SIZE);

    streamAnalyzer.addStreamDetector(&riffWaveDetector);

    try {
        streamAnalyzer.execute();
        streamCompressor.execute(streamAnalyzer.getStreamList());
    } catch (const std::exception& err) {
        std::cerr << "[!] " << err.what() << std::endl;
        return 1;
    }

    fsPtr->close();

    return 0;
}