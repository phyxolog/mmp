#pragma once

#include <experimental/filesystem>

#include "Types.hpp"

namespace marc {
    namespace fs = std::experimental::filesystem;

    namespace fs_types {
        enum { readMode, writeMode, appendMode };
        enum { fileBegin, fileCurrent, fileEnd };
        enum { Normal, SequentialScan, RandomAccess };
    };

    class FS
    {
    private:
        fs::path path;
        handle hFile;
        dword getFileFlag(dword flag) const;

    public:
        FS();
        FS(fs::path path, short int openMode = fs_types::readMode, dword flag = fs_types::Normal);
        int64 getFileSize();
        bool open(fs::path path, short int openMode, dword flag);
        int64 seek(int64 offset, short int moveMethod = fs_types::fileBegin);
        bool read(uint size, void *cBuffer);
        bool write(uint size, void *cBuffer);
        fs::path getAbsolutePath();
        void close();
    };
}