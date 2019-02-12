#pragma once

#include <Windows.h>
#include <experimental/filesystem>

namespace mmp {
    namespace fs = std::experimental::filesystem;

    namespace fs_types {
        enum { readMode, writeMode, appendMode };
        enum { fileBegin, fileCurrent, fileEnd };
        enum { Normal, SequentialScan, RandomAccess };
    };

    class FileStream {
    private:
        fs::path path;
        HANDLE hFile;
        DWORD getFileFlag(DWORD flag) const;

    public:
        FileStream();
        FileStream(fs::path path, short int openMode = fs_types::readMode, DWORD flag = fs_types::Normal);
        uintmax_t getFileSize();
        bool open(fs::path path, short int openMode, DWORD flag);
        uintmax_t seek(uintmax_t offset, short int moveMethod = fs_types::fileBegin);
        bool read(unsigned int size, void *cBuffer);
        bool write(unsigned int size, void *cBuffer);
        fs::path getAbsolutePath();
        void close();
    };
}
