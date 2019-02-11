#pragma once

#include <experimental/filesystem>
#include <list>
#include <sstream>

#include "Stream.hpp"
#include "FS.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace marc {
    class StreamCompressor {
    private:
        uint bufferSize;
        marc::FS *filePtr, *cFilePtr;

        void CompressStream(Stream&, MarcCompressedStream&, fs::path&);
        bool TakCompress(fs::path, fs::path&, unsigned short);

    public:
        StreamCompressor(marc::FS *filePtr, marc::FS *cFilePtr, uint bufferSize);
        ~StreamCompressor();

        void execute(std::list<Stream>&);
    };
}