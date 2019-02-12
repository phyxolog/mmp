#pragma once

#include <string>
#include <cstdint>

namespace mmp {
    class BaseStream {
    private:
        uintmax_t size = 0;
        uintmax_t offset = 0;

    public:
        void setSize(uintmax_t _size) {
            size = _size;
        }

        void setOffset(uintmax_t _offset) {
            offset = _offset;
        }

        uintmax_t getOffset() const {
            return offset;
        }

        uintmax_t getSize() const {
            return size;
        }

        virtual std::string getFileExtension() = 0;
        virtual std::string getTypeAsString() = 0;
        virtual std::string getTypeAsShortString() = 0;
    };
}