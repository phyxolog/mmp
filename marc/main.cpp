#include <iostream>
#include <experimental/filesystem>

#include "pch.h"
#include "Types.h"
#include "FS.hpp"
#include "TurboBM.hpp"

namespace fs = std::experimental::filesystem;

#define DEFAULT_BUFFER_SIZE 1 * 1024 * 1024

void findOffsets(const char *buffer, uint bufferSize, int64 currentOffset) {
    marc::skiptableType skiptableType = marc::TurboSearch::CreateSkipTable("RIFF", 4);
    marc::occtableType occtableType = marc::TurboSearch::CreateOccTable("RIFF", 4);

    uint index = marc::TurboSearch::Search(buffer, bufferSize, occtableType, skiptableType, "RIFF", 4);

    while (index != -1) {
        std::cout << "Found RIFF WAVE at position: " << currentOffset + index << std::endl;

        index = marc::TurboSearch::Search(buffer, bufferSize, occtableType, skiptableType, "RIFF", 4, index + 1);
    }
}

void startScan(uint bufferSize, marc::FS *fsPtr) {
    int64 fileSize = fsPtr->getFileSize();
    int64 currentOffset = 0;
    uint maxSize = bufferSize;
    short int rollbackSize = 16;
    char *buffer = new char[bufferSize];

    while (currentOffset < fileSize) {
        if (maxSize != bufferSize) {
            rollbackSize = 0;
        }

        fsPtr->seek(currentOffset);

        if (currentOffset + bufferSize > fileSize) {
            bufferSize = static_cast<uint>(fileSize - currentOffset);
        }

        fsPtr->read(bufferSize, buffer);

        findOffsets(buffer, bufferSize, currentOffset);

        currentOffset += bufferSize - rollbackSize;
    }

    delete[] buffer;
    return;
}

int main() {
    fs::path path("D:\\Projects\\marc\\Debug\\stored.zip");
    marc::FS *fsPtr = new marc::FS(path, marc::fs_types::readMode, marc::fs_types::SequentialScan);
    
    startScan(DEFAULT_BUFFER_SIZE, fsPtr);

    return 0;
}