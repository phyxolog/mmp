#pragma once

#include <experimental/filesystem>

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

        static void CopyData(marc::FS *src, marc::FS *dst, uintmax_t offset, uintmax_t size) {
            uint bufferSize = 256 * 1024;
            int64 readBytes = 0;
            src->seek(offset, marc::fs_types::fileBegin);

            if (size < bufferSize) {
                bufferSize = static_cast<unsigned int>(size);
            }

            char *buffer = new char[bufferSize];

            while (readBytes < size) {
                if ((readBytes + bufferSize) > size) {
                    bufferSize = static_cast<unsigned int>(size - readBytes);
                }

                src->read(bufferSize, buffer);
                dst->write(bufferSize, buffer);

                readBytes += bufferSize;
            }

            delete[] buffer;
        }

        static void ZeroFile(marc::FS *src, uint size) {
            char *nulls = new char[size];
            memset(nulls, 0, size);
            src->write(size, nulls);
            delete[] nulls;
        }

        static int execAndWait(const std::string &exePath, const std::string &args) {
            // For redirect stdout
            SECURITY_ATTRIBUTES SecurityAttributes;
            SecurityAttributes.nLength = sizeof(SecurityAttributes);
            SecurityAttributes.lpSecurityDescriptor = NULL;
            SecurityAttributes.bInheritHandle = TRUE;

            HANDLE hFile = ::CreateFile("NUL",
                FILE_APPEND_DATA,
                FILE_SHARE_WRITE | FILE_SHARE_READ,
                &SecurityAttributes,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            PROCESS_INFORMATION ProcInfo;
            STARTUPINFOA StartupInfo;

            memset(&ProcInfo, 0, sizeof(ProcInfo));
            memset(&StartupInfo, 0, sizeof(StartupInfo));

            StartupInfo.cb = sizeof(StartupInfo);
            StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
            StartupInfo.wShowWindow = SW_SHOW;
            StartupInfo.hStdInput = NULL;
            StartupInfo.hStdError = hFile;
            StartupInfo.hStdOutput = hFile;
           
            LPCSTR _exePath = exePath.c_str();
            std::string sargs = exePath + " " + args;
            LPSTR _args = (LPSTR)(sargs.c_str());

            if (!::CreateProcess(_exePath, _args, nullptr, nullptr, true, 0,
                nullptr, nullptr, &StartupInfo, &ProcInfo)) {
                return -1;
            }

            dword exitCode;
            ::WaitForSingleObject(ProcInfo.hProcess, INFINITE);
            ::GetExitCodeProcess(ProcInfo.hProcess, &exitCode);

            ::CloseHandle(ProcInfo.hProcess);
            ::CloseHandle(ProcInfo.hThread);

            return exitCode;
        }
    };
}