#pragma once

#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

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