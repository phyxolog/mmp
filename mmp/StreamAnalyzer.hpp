#pragma once

#include <list>
#include <iterator>
#include <algorithm>

#include "FileStream.hpp"
#include "BaseStream.hpp"
#include "IStreamDetector.hpp"

namespace mmp {
    class StreamAnalyzer {
    private:
        unsigned int bufferSize;
        mmp::FileStream *filePtr;
        std::list<IStreamDetector*> streamDetectorList;
        std::list<BaseStream*> streamList;

    public:
        StreamAnalyzer(mmp::FileStream *filePtr, unsigned int bufferSize);
        ~StreamAnalyzer();

        void addStreamDetector(IStreamDetector *streamDetector);
        void execute();

        std::list<BaseStream*>& getStreamList();
    };
}
