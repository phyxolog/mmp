#include "pch.h"
#include "StreamAnalyzer.hpp"

namespace marc {
    StreamAnalyzer::StreamAnalyzer(marc::FS *filePtr, uint bufferSize) : filePtr(filePtr), bufferSize(bufferSize) { }
    StreamAnalyzer::~StreamAnalyzer() { }

    void StreamAnalyzer::addStreamDetector(IStreamDetector *streamDetector) {
        streamDetectorList.push_front(streamDetector);
    }

    void StreamAnalyzer::execute() {
        if (streamDetectorList.size() == 0) {
            throw std::exception("Empty stream detector list!");
        }

        std::list<Stream> localStreamList;
        int64 fileSize = filePtr->getFileSize();
        int64 currentOffset = 0;
        uint maxSize = bufferSize;
        short int rollbackSize = 16;
        char *buffer = new char[bufferSize];

        while (currentOffset < fileSize) {
            if (maxSize != bufferSize) {
                rollbackSize = 0;
            }

            filePtr->seek(currentOffset);

            if (currentOffset + bufferSize > fileSize) {
                bufferSize = static_cast<uint>(fileSize - currentOffset);
            }

            filePtr->read(bufferSize, buffer);

            for (auto streamDetector : streamDetectorList) {
                localStreamList.clear();
                streamDetector->execute(buffer, bufferSize, currentOffset, localStreamList);

                if (!localStreamList.empty()) {
                    streamDetector->analyzeOffsets(filePtr, localStreamList);
                }

                std::copy(localStreamList.rbegin(), localStreamList.rend(), front_inserter(streamList));
            }

            currentOffset += bufferSize - rollbackSize;
        }

        for (auto stream : streamList) {
            std::cout << "Stream: " << stream.getType()
                << ", Size: " << stream.getSize()
                << ", Offset: " << stream.getOffset()
                << std::endl;
        }

        delete[] buffer;
        return;
    }
}