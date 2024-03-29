#include "pch.hpp"

#include <experimental/filesystem>

#include "RiffWaveStream.hpp"

#include "RiffWaveDetector.hpp"
#include "StreamAnalyzer.hpp"
#include "StreamCompressor.hpp"
#include "FileStream.hpp"
#include "State.hpp"

namespace fs = std::experimental::filesystem;

int main() {
    // std::cout << sizeof(mmp::RiffWaveStream) << std::endl;
    // std::cout << mmp::State::instance()->analyzerOptions["wav"] << std::endl;

    fs::path path("D:\\Projects\\mmp\\Debug\\stored.zip");
    mmp::FileStream *file = new mmp::FileStream(path, mmp::fs_types::readMode, mmp::fs_types::RandomAccess);

    fs::path out("D:\\Projects\\mmp\\Debug\\stored-packed.mcf");
    mmp::FileStream *cFile = new mmp::FileStream(out, mmp::fs_types::writeMode);

    // Initialize detectors
    mmp::RiffWaveDetector *riffWaveDetector = new mmp::RiffWaveDetector();

    mmp::StreamAnalyzer streamAnalyzer(file, 1 * 1024 * 1024);
    mmp::StreamCompressor streamCompressor(file, cFile, 1 * 1024 * 1024);

    streamAnalyzer.addStreamDetector(riffWaveDetector);

    //try {
        streamAnalyzer.execute();
        streamCompressor.execute(streamAnalyzer.getStreamList());
    /*} catch (const std::exception& err) {
        std::cerr << "[!] " << err.what() << std::endl;
        return 1;
    }*/

    file->close();
    cFile->close();

    return 0;
}