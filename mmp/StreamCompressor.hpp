#pragma once

#include <experimental/filesystem>
#include <list>
#include <sstream>

#include "Stream.hpp"
#include "FS.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace mmp {
    class StreamCompressor {
    private:
        uint bufferSize;
        mmp::FS *filePtr, *cFilePtr;

        void CompressStream(Stream&, MarcCompressedStream&, fs::path&);
        bool TakCompress(fs::path, fs::path&, unsigned short);

    public:
        StreamCompressor(mmp::FS *filePtr, mmp::FS *cFilePtr, uint bufferSize);
        ~StreamCompressor();

        void execute(std::list<Stream>&);
    };
}
