#pragma once

#include <list>
#include <string>
#include <cstdint>

#include "Types.hpp"

namespace mmp {
    class BaseStream {
    private:
        uintmax_t size = 0;
        uintmax_t offset = 0;
        std::list<DiffType> diffList;

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

        void addDiff(DiffType dt) {
            diffList.push_front(dt);
        }

        std::list<DiffType>& getDiffList() {
            return diffList;
        }

        virtual std::string getFileExtension() = 0;
        virtual std::string getTypeAsString() = 0;
        virtual std::string getTypeAsShortString() = 0;
    };
}