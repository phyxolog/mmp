#pragma once

#include <experimental/filesystem>
#include <list>
#include <sstream>

#include "Stream.hpp"
#include "FS.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace mmp {
    typedef struct MarcCompressedStream {
        StreamType type;
        CompressorType compressor;
        int64 nextCompressedStreamOffset;
        int64 compressedOffset;
        int64 compressedSize;
        int64 originalOffset;
        int64 originalSize;
        uint32_t originalCRC32;
    } MarcCompressedStream;

#pragma pack(push, 1)
    typedef struct MarcHeader {
        char signature[3];
        char version[3];
        uint32_t crc32;
        int64 firstCompressedStreamOffset;
    } MarcHeader;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct MarcInternalCompressedStream {
        StreamType type;
        CompressorType compressor;
        int64 nextCompressedStreamOffset;
        int64 compressedSize;
        int64 originalOffset;
        uint32_t originalCRC32;
    } MarcInternalCompressedStream;
#pragma pack(pop)

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
