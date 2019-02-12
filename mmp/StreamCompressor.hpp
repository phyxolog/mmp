#pragma once

#include <experimental/filesystem>
#include <list>
#include <sstream>

#include "Types.hpp"
#include "BaseStream.hpp"
#include "FileStream.hpp"
#include "Utils.hpp"
#include "State.hpp"

namespace mmp {
    const char Signature[3] = { 'M', 'C', 'F' };
    const char Version[3]   = { '0', '0', '1' };

    typedef struct MetaData {
        char type[3];
        char compressor[3];
    } MetaData;

    typedef struct CompressedStream {
        MetaData metaData;
        uintmax_t nextCompressedStreamOffset;
        uintmax_t compressedOffset;
        uintmax_t compressedSize;
        uintmax_t originalOffset;
        uintmax_t originalSize;
        uint32_t originalCRC32;
    } CompressedStream;

#pragma pack(push, 1)
    typedef struct PackerHeader {
        char signature[3];
        char version[3];
        uint32_t crc32;
        uintmax_t firstCompressedStreamOffset;
    } PackerHeader;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct InternalCompressedStream {
        MetaData metaData;
        uintmax_t nextCompressedStreamOffset;
        uintmax_t compressedSize;
        uintmax_t originalOffset;
        uint32_t originalCRC32;
    } InternalCompressedStream;
#pragma pack(pop)

    class StreamCompressor {
    private:
        unsigned int bufferSize;
        mmp::FileStream *filePtr, *cFilePtr;

        void CompressStream(BaseStream&, CompressedStream&, fs::path&);
        bool TakCompress(fs::path, fs::path&, unsigned short);

    public:
        StreamCompressor(mmp::FileStream *filePtr, mmp::FileStream *cFilePtr, unsigned int bufferSize);
        ~StreamCompressor();

        void execute(std::list<BaseStream*>&);
    };
}
