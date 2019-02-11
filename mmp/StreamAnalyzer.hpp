#pragma once

#include <list>
#include <iterator>
#include <algorithm>
#include <functional>

#include "FS.hpp"
#include "Types.hpp"
#include "Stream.hpp"
#include "IStreamDetector.hpp"

namespace mmp {
    class StreamAnalyzer {
    private:
        uint bufferSize;
        mmp::FS *filePtr;
        std::list<IStreamDetector*> streamDetectorList;
        std::list<Stream> streamList;

    public:
        StreamAnalyzer(mmp::FS *filePtr, uint bufferSize);
        ~StreamAnalyzer();

        void addStreamDetector(IStreamDetector *streamDetector);
        void execute();

        std::list<Stream>& getStreamList();
    };
}
