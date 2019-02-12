#include "pch.hpp"
#include "FileStream.hpp"

namespace mmp {
    FileStream::FileStream() { }
    FileStream::FileStream(fs::path path, short int openMode, DWORD flag) : path(path) {
        open(path, openMode, flag);
    }

    uintmax_t FileStream::getFileSize() {
        return fs::file_size(path);
    }

    DWORD FileStream::getFileFlag(DWORD flag) const {
        switch (flag)
        {
            case fs_types::Normal:         flag = FILE_ATTRIBUTE_NORMAL;     break;
            case fs_types::SequentialScan: flag = FILE_FLAG_SEQUENTIAL_SCAN; break;
            case fs_types::RandomAccess:   flag = FILE_FLAG_RANDOM_ACCESS;   break;
            default: break;
        }

        return flag;
    }

    bool FileStream::open(fs::path path, short int openMode, DWORD flag) {
        path = path;
        hFile = ::CreateFile(
            path.string().c_str(),
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

    uintmax_t FileStream::seek(uintmax_t offset, short int moveMethod) {
        LARGE_INTEGER li;
        li.QuadPart = offset;

        li.LowPart = ::SetFilePointer(hFile, li.LowPart, &li.HighPart, moveMethod);

        if (li.LowPart == INVALID_SET_FILE_POINTER) {
            li.QuadPart = -1;
        }

        return li.QuadPart;
    }

    bool FileStream::read(unsigned int size, void *cBuffer) {
        DWORD numberOfBytesRead;

        bool result = ::ReadFile(hFile, cBuffer, size, &numberOfBytesRead, nullptr);
        if (!result || numberOfBytesRead != size) {
            return false;
        }

        return true;
    }

    bool FileStream::write(unsigned int size, void *cBuffer) {
        DWORD numberOfBytesWritten;

        bool result = ::WriteFile(hFile, cBuffer, size, &numberOfBytesWritten, nullptr);
        if (!result || numberOfBytesWritten != size) {
            return false;
        }

        return true;
    }

    fs::path FileStream::getAbsolutePath() {
        return fs::absolute(path);
    }

    void FileStream::close() {
        if (hFile) {
            ::CloseHandle(hFile);
        }
    }
}
