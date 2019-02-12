#include "pch.hpp"
#include "StreamAnalyzer.hpp"

namespace mmp {
    StreamAnalyzer::StreamAnalyzer(mmp::FileStream *filePtr, unsigned int bufferSize)
        : filePtr(filePtr), bufferSize(bufferSize) { }
    StreamAnalyzer::~StreamAnalyzer() { }

    void StreamAnalyzer::addStreamDetector(IStreamDetector *streamDetector) {
        streamDetectorList.push_front(streamDetector);
    }

    void StreamAnalyzer::execute() {
        if (streamDetectorList.size() == 0) {
            throw std::exception("Empty stream detector list!");
        }

        std::list<BaseStream*> localStreamList;
        uintmax_t fileSize = filePtr->getFileSize();
        uintmax_t currentOffset = 0;
        unsigned int maxSize = bufferSize;
        short int rollbackSize = 16;
        char *buffer = new char[bufferSize];

        while (currentOffset < fileSize) {
            if (maxSize != bufferSize) {
                rollbackSize = 0;
            }

            filePtr->seek(currentOffset);

            if (currentOffset + bufferSize > fileSize) {
                bufferSize = static_cast<unsigned int>(fileSize - currentOffset);
            }

            filePtr->read(bufferSize, buffer);

            for (auto streamDetector : streamDetectorList) {
                localStreamList.clear();
                streamDetector->execute(buffer, bufferSize, currentOffset, localStreamList);

                if (!localStreamList.empty()) {
                    streamDetector->analyze(filePtr, localStreamList);
                }

                std::copy(localStreamList.rbegin(), localStreamList.rend(), front_inserter(streamList));
            }

            currentOffset += bufferSize - rollbackSize;
        }

        delete[] buffer;
        return;
    }

    std::list<BaseStream*>& StreamAnalyzer::getStreamList() {
        return streamList;
    }
}
