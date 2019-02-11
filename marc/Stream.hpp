#pragma once

#include "Types.hpp"

namespace marc {
    class Stream {
    private:
        int64 size = 0;
        int64 offset = 0;
        StreamType type = 0;

    public:
        void setSize(int64 _size) {
            size = _size;
        }

        void setOffset(int64 _offset) {
            offset = _offset;
        }

        void setType(StreamType _type) {
            type = _type;
        }

        int64 getOffset() const {
            return offset;
        }

        int64 getSize() const {
            return size;
        }

        StreamType getType() const {
            return type;
        }
    };
}