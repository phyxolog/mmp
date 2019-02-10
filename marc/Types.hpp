#pragma once

#include <cstdint>

namespace marc {
    typedef uintmax_t int64;
    typedef unsigned int uint;
    typedef unsigned long dword;
    typedef void* handle;
    typedef short int Type;

    const char marcSignature[3] = { 'M', 'C', 'F' };
    const char marcVersion[3] = { '0', '0', '1' };

#pragma pack(push, 1)
    typedef struct MarcHeader {
        char Signature[3];
        char Version[3];
        int64 OriginalSize;
        uint32_t OriginalCRC32;
        int64 FirstCompressedStreamOffset;
    } MarcHeader;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct MarcCompressedStream {
        unsigned char Type;
        unsigned char Compressor;
        int64 NextCompressedStreamOffset;
        int64 CompressedSize;
        int64 OriginalOffset;
        int64 OriginalSize;
        uint32_t OriginalCRC32;
    } MarcCompressedStream;
#pragma pack(pop)

    enum { RIFF_WAVE_TYPE = 1 };
}