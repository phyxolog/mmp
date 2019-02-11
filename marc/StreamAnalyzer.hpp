#pragma once

#include <list>
#include <iterator>
#include <algorithm>

#include "FS.hpp"
#include "Types.hpp"
#include "Stream.hpp"
#include "IStreamDetector.hpp"

namespace marc {
    class StreamAnalyzer {
    private:
        uint bufferSize;
        marc::FS *filePtr;
        std::list<IStreamDetector*> streamDetectorList;
        std::list<Stream> streamList;

    public:
        StreamAnalyzer(marc::FS *filePtr, uint bufferSize);
        ~StreamAnalyzer();

        void addStreamDetector(IStreamDetector *streamDetector);
        void execute();

        std::list<Stream>& getStreamList();
    };
}