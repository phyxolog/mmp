#include "pch.h"
#include "FS.hpp"

namespace marc {

    FS::FS() { }
    FS::FS(fs::path path, short int openMode, dword flag) : path(path) {
        open(path, openMode, flag);
    }

    int64 FS::getFileSize() {
        return fs::file_size(path);
    }

    dword FS::getFileFlag(dword flag) const {
        switch (flag)
        {
            case fs_types::Normal:         flag = FILE_ATTRIBUTE_NORMAL;     break;
            case fs_types::SequentialScan: flag = FILE_FLAG_SEQUENTIAL_SCAN; break;
            case fs_types::RandomAccess:   flag = FILE_FLAG_RANDOM_ACCESS;   break;
            default: break;
        }

        return flag;
    }

    bool FS::open(fs::path path, short int openMode, dword flag) {
        path = path;
        hFile = ::CreateFile(
            path.c_str(),
            openMode == fs_types::readMode ? GENERIC_READ : GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            openMode == fs_types::readMode
                ? OPEN_EXISTING
                : openMode == fs_types::writeMode
                    ? CREATE_ALWAYS
                    : OPEN_ALWAYS,
            getFileFlag(flag),
            nullptr
        );

        if (hFile == INVALID_HANDLE_VALUE) {
            return false;
        }

        return true;
    }

    bool FS::seek(int64 offset, short int moveMethod) {
        LARGE_INTEGER li;
        li.QuadPart = offset;

        dword result = ::SetFilePointer(hFile, li.LowPart, &li.HighPart, moveMethod);

        if (result == INVALID_SET_FILE_POINTER) {
            return false;
        }

        return true;
    }

    bool FS::read(uint size, void *cBuffer) {
        DWORD numberOfBytesRead;

        bool result = ::ReadFile(hFile, cBuffer, size, &numberOfBytesRead, nullptr);
        if (!result || numberOfBytesRead != size) {
            return false;
        }

        return true;
    }

    bool FS::write(uint size, void *cBuffer) {
        DWORD numberOfBytesWritten;

        bool result = ::WriteFile(hFile, cBuffer, size, &numberOfBytesWritten, nullptr);
        if (!result || numberOfBytesWritten != size) {
            return false;
        }

        return true;
    }

    void FS::close() {
        if (hFile) {
            ::CloseHandle(hFile);
        }
    }

}