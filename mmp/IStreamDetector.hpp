#pragma once

#include <list>

#include "FS.hpp"
#include "Stream.hpp"

namespace mmp {
    class IStreamDetector {
    private:
        virtual bool validHeader(const char *, size_t) = 0;
    public:
        IStreamDetector() { }
        virtual ~IStreamDetector() { }
        virtual void execute(const char *, uint, int64, std::list<Stream>&) = 0;
        virtual void analyze(FS*, std::list<Stream>&) = 0;
    };
}
