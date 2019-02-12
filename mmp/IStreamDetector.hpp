#pragma once

#include <functional>
#include <list>

#include "FileStream.hpp"
#include "BaseStream.hpp"

namespace mmp {
    class IStreamDetector {
    private:
        virtual bool validHeader(const char *, size_t) = 0;
    public:
        IStreamDetector() { }
        virtual ~IStreamDetector() { }
        virtual void execute(const char *, unsigned int, uintmax_t, std::list<BaseStream*>&) = 0;
        virtual void analyze(FileStream*, std::list<BaseStream*>&) = 0;
    };
}
