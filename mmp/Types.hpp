#pragma once

#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

#define RIFF_WAVE_SHORT_TYPE "RW"

namespace mmp {
#pragma pack(push, 1)
    typedef struct DiffType {
        unsigned int size;
        uintmax_t offset;
        void *oldData;
        void *newData;
    } DiffType;
#pragma pack(pop)
}

#endif // TYPES_HPP