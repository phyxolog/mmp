#pragma once

#include <boost/format.hpp>

#include <experimental/filesystem>
#include <map>
#include <algorithm>
#include <chrono>

#include "CRC32.hpp"
#include "FS.hpp"
#include "Types.hpp"

namespace mmp {
    namespace fs = std::experimental::filesystem;

    class Utils {
    public:
        static std::string humanizeSize(uintmax_t Bytes) {
            if (Bytes == 0) return "0 B";
            int exp = static_cast<int>(log(Bytes) / log(1024));
            std::string prefix = std::string("BKMGTPE").substr(static_cast<unsigned long>(exp), 1) + (exp == 0 ? "" : "b");
            return boost::str(boost::format("%.2f %s") % (Bytes / pow(1024, exp)) % prefix);
        }

        static std::string generateUniqueFolderName(std::string firstPrefix, std::string secondPrefix) {
            return std::string(firstPrefix + "_" + secondPrefix + "_" + std::to_string(std::chrono::seconds(std::time(nullptr)).count()));
        }

        static long long memToll(std::string str) {
            if (str.length() == 0) {
                return 0;
            }

            size_t nondigit_pos = str.find_first_not_of("0123456789");

            if (nondigit_pos == std::string::npos) {
                return std::stoll(str);
            }

            long long result = 0;
            std::string digits = str.substr(0, nondigit_pos);
            std::string u = str.substr(nondigit_pos, str.length());
            std::transform(u.begin(), u.end(), u.begin(), ::tolower);

            if (digits.length() == 0) {
                return result;
            }
            else {
                result = std::stoll(digits);
            }

            std::map<std::string, long> umul = {
                { "b",  1                   },
                { "k",  1000                },
                { "kb", 1024                },
                { "m",  1000 * 1000        },
                { "mb", 1024 * 1024        },
                { "g",  1000L * 1000 * 1000 },
                { "gb", 1024L * 1024 * 1024 },
            };

            auto mul = umul.find(u);
            if (mul != umul.end()) {
                return result * mul->second;
            }
            else {
                return result;
            }
        }

        static uint32_t calculateCrc32(mmp::FS *filePtr, int64 offset, int64 size) {
            uint bufferSize = 16 * 1024 * 1024;
            int64 readBytes = 0;
            filePtr->seek(offset, mmp::fs_types::fileBegin);

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

        static void extractFileFromStream(mmp::FS *filePtr, int64 offset, int64 size, std::string outFileName) {
            uint bufferSize = 256 * 1024;
            filePtr->seek(offset, mmp::fs_types::fileBegin);

            if (size < bufferSize) {
                bufferSize = static_cast<unsigned int>(size);
            }

            int64 readBytes = 0;
            char *buffer = new char[bufferSize];
            mmp::FS *outFile = new mmp::FS(outFileName, mmp::fs_types::writeMode);

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

        static void CopyData(mmp::FS *src, mmp::FS *dst, uintmax_t offset, uintmax_t size) {
            uint bufferSize = 256 * 1024;
            int64 readBytes = 0;
            src->seek(offset, mmp::fs_types::fileBegin);

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

        static void ZeroFile(mmp::FS *src, uint size) {
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
