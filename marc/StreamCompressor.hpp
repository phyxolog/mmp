#pragma once

#include <experimental/filesystem>
#include <list>

#include "Stream.hpp"
#include "FS.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace marc {
    class StreamCompressor {
    private:
        uint bufferSize;
        marc::FS *filePtr;

    public:
        StreamCompressor(marc::FS *filePtr, uint bufferSize);
        ~StreamCompressor();

        void execute(std::list<Stream>&);

        //bool WavpackCompress(fs::path, fs::path, unsigned short);
        //bool TakCompress(fs::path, fs::path, unsigned short);
    };
}