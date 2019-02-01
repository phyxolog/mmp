#include "pch.h"
#include <experimental/filesystem>
#include <list>

#include "StreamAnalyzer.hpp"
#include "RiffWaveDetector.hpp"

namespace fs = std::experimental::filesystem;

#define DEFAULT_BUFFER_SIZE 1 * 1024 * 1024

/*
Stream interface example:

class IStream {
private:
    int64 size;
    int64 offset;
    Format format;
    const char *data;

public:
    void setSize(int64 _size) {
        size = _size;
    }

    void setOffset(int64 _offset) {
        offset = _offset;
    }

    void setFormat(Format _format) {
        format = _format;
    }

    void setData(const char * _data) {
        /// memcpy
    }

    virtual ~IStream() {}
    virtual int64 streamOffset() = 0;
    virtual int64 streamSize() = 0;
    virtual Format streamFormat() = 0;
    virtual const char *streamData() = 0;
};

class RiffWave : public IStream {
public:
    /// etc...
};

class Bitmap : public IStream {
public:
    /// etc...
};

*/

int main() {
    fs::path path("D:\\Projects\\marc\\Debug\\stored.zip");
    marc::FS *fsPtr = new marc::FS(path, marc::fs_types::readMode, marc::fs_types::SequentialScan);

    // Initialize detectors
    marc::RiffWaveDetector riffWaveDetector;
    
    marc::StreamAnalyzer streamAnalyzer(fsPtr, DEFAULT_BUFFER_SIZE);

    streamAnalyzer.addStreamDetector(&riffWaveDetector);

    try {
        streamAnalyzer.execute();
    } catch (const std::exception& err) {
        std::cerr << "[!] " << err.what() << std::endl;
        return 1;
    }

    return 0;
}