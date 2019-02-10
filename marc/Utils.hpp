#pragma once

#include <experimental/filesystem>

#include "pch.h"
#include "CRC32.hpp"
#include "FS.hpp"
#include "Types.hpp"

namespace marc {
    namespace fs = std::experimental::filesystem;

    class Utils {
    public:
        static uint32_t calculateCrc32(marc::FS *filePtr, int64 offset, int64 size) {
            uint bufferSize = 16 * 1024 * 1024;
            int64 readBytes = 0;
            filePtr->seek(offset, marc::fs_types::fileBegin);

            if (size < bufferSize) {
                bufferSize = static_cast<uint>(size);
            }

            char *buffer = new char[bufferSize];
            uint32_t crc32 = 0;

            while (readBytes < size) {
                if ((readBytes + bufferSize) > size) {
                    bufferSize = static_cast<unsigned int>(size - readBytes);
                }

                filePtr->read(bufferSize, buffer);
                crc32 = CRC32::update(crc32, buffer, bufferSize);

                readBytes += bufferSize;
            }

            return crc32;
        }

        static void extractFileFromStream(marc::FS *filePtr, int64 offset, int64 size, std::string outFileName) {
            uint bufferSize = 256 * 1024;
            filePtr->seek(offset, marc::fs_types::fileBegin);

            if (size < bufferSize) {
                bufferSize = static_cast<unsigned int>(size);
            }

            int64 readBytes = 0;
            char *buffer = new char[bufferSize];
            marc::FS *outFile = new marc::FS(outFileName, marc::fs_types::writeMode);

            while (readBytes < size) {
                if ((readBytes + bufferSize) > size) {
                    bufferSize = static_cast<unsigned int>(size - readBytes);
                }

                filePtr->read(bufferSize, buffer);
                outFile->write(bufferSize, buffer);
                readBytes += bufferSize;
            }

            delete[] buffer;
            outFile->close();
        }

        bool execAndWait(const fs::path &exePath) {
            // For redirect stdout
            SECURITY_ATTRIBUTES SecurityAttributes;
            SecurityAttributes.nLength = sizeof(SecurityAttributes);
            SecurityAttributes.lpSecurityDescriptor = NULL;
            SecurityAttributes.bInheritHandle = TRUE;

            HANDLE hFile = CreateFile(L"NUL",
                FILE_APPEND_DATA,
                FILE_SHARE_WRITE | FILE_SHARE_READ,
                &SecurityAttributes,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            PROCESS_INFORMATION ProcInfo;
            STARTUPINFO StartupInfo;

            memset(&ProcInfo, 0, sizeof(ProcInfo));
            memset(&StartupInfo, 0, sizeof(StartupInfo));

            StartupInfo.cb = sizeof(StartupInfo);
            StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
            StartupInfo.wShowWindow = SW_SHOW;
            StartupInfo.hStdInput = NULL;
            StartupInfo.hStdError = hFile;
            StartupInfo.hStdOutput = hFile;

            if (!CreateProcess((LPCWSTR)(exePath.u8string().c_str()), nullptr, nullptr, nullptr, false, 0,
                nullptr, nullptr, &StartupInfo, &ProcInfo)) {
                return false;
            }

            CloseHandle(ProcInfo.hProcess);
            CloseHandle(ProcInfo.hThread);

            return true;
        }
    };
}