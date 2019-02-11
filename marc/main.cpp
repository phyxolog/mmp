#include <experimental/filesystem>
#include <list>

#include "pch.hpp"
#include "StreamAnalyzer.hpp"
#include "RiffWaveDetector.hpp"
#include "StreamCompressor.hpp"

namespace fs = std::experimental::filesystem;

#define DEFAULT_BUFFER_SIZE 1 * 1024 * 1024

int main() {
    fs::path path("D:\\Projects\\marc\\Debug\\stored.zip");
    marc::FS *file = new marc::FS(path, marc::fs_types::readMode, marc::fs_types::RandomAccess);

    fs::path out("D:\\Projects\\marc\\Debug\\stored-packed.mcf");
    marc::FS *cFile = new marc::FS(out, marc::fs_types::writeMode);

    // Initialize detectors
    marc::RiffWaveDetector riffWaveDetector;
    
    marc::StreamAnalyzer streamAnalyzer(file, DEFAULT_BUFFER_SIZE);
    marc::StreamCompressor streamCompressor(file, cFile, DEFAULT_BUFFER_SIZE);

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