#pragma once

#include <cstdint>

namespace marc {
    typedef uintmax_t int64;
    typedef unsigned int uint;
    typedef unsigned long dword;
    typedef void* handle;
    typedef short int StreamType;
    typedef short int CompressorType;

    const std::string StreamExts[] = { "wav" };

    const char Signature[3] = { 'M', 'C', 'F' };
    const char Version[3] = { '0', '0', '1' };

#pragma pack(push, 1)
    typedef struct MarcHeader {
        char signature[3];
        char version[3];
        uint32_t crc32;
        int64 firstCompressedStreamOffset;
    } MarcHeader;
#pragma pack(pop)

#pragma pack(push, 1)
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

    enum { RIFF_WAVE_TYPE = 0 };
}