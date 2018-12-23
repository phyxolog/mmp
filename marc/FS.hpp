#pragma once

#include <experimental/filesystem>

namespace marc {
    namespace fs = std::experimental::filesystem;
    typedef uintmax_t int64;
    typedef unsigned int uint;
    typedef unsigned long dword;
    typedef void* handle;

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
        bool open(fs::path path, short int openMode, dword flag);
        bool seek(int64 offset, short int moveMethod = fs_types::fileBegin);
        bool read(uint size, void *cBuffer);
        bool write(uint size, void *cBuffer);
        void close();
    };
}