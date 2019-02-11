#pragma once

#include <experimental/filesystem>
#include <cstdint>
#include <string>

namespace mmp {
    namespace fs = std::experimental::filesystem;

    typedef uintmax_t int64;
    typedef unsigned int uint;
    typedef unsigned long dword;
    typedef void* handle;
    typedef short int StreamType;
    typedef short int CompressorType;
    typedef std::string StreamStringType;

    const StreamStringType StreamTypes[] = { "RIFF WAVE" };
    const std::string      StreamExts[] = { "wav" };

    const char Signature[3] = { 'M', 'C', 'F' };
    const char Version[3] = { '0', '0', '1' };

    typedef struct CLIOptions {
        fs::path outDir;
        std::string command;
        fs::path inFile;
        fs::path outFile;
        uint bufferSize;
        bool verbose;
        bool enableRiffWaveAnalyze;
        unsigned short takCompLevel;
    } CLIOptions;

    enum { RIFF_WAVE_TYPE = 0 };
}
