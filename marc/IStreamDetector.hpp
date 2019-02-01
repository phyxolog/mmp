#pragma once

#include <list>

#include "FS.hpp"
#include "Stream.hpp"

namespace marc {
    class IStreamDetector {
    private:
        virtual bool validHeader(const char *, uint) = 0;
    public:
        virtual ~IStreamDetector() { }
        virtual void execute(const char *, uint, int64, std::list<Stream>&) = 0;
        virtual void analyze(FS*, std::list<Stream>&) = 0;
    };
}