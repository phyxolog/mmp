#pragma once

#include <list>

#include "Types.hpp"
#include "FS.hpp"
#include "Stream.hpp"

namespace mmp {
    typedef const std::function<void(const Stream&)> detectorCallbackHandle;

    class IStreamDetector {
    protected:
        detectorCallbackHandle *callback;
    private:
        virtual bool validHeader(const char *, size_t) = 0;
    public:
        IStreamDetector() { }
        virtual ~IStreamDetector() { }
        virtual void execute(const char *, uint, int64, std::list<Stream>&) = 0;
        virtual void analyze(FS*, std::list<Stream>&) = 0;

        void setCallback(void *_callback) {
            callback = static_cast<detectorCallbackHandle*>(_callback);
        }
    };
}
