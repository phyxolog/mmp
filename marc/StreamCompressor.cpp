#include "pch.h"
#include "StreamCompressor.hpp"

namespace marc {
    namespace fs = std::experimental::filesystem;

    StreamCompressor::StreamCompressor(marc::FS *filePtr, uint bufferSize) : filePtr(filePtr), bufferSize(bufferSize) { }
    StreamCompressor::~StreamCompressor() { }

    void StreamCompressor::execute(std::list<Stream> &streamList) {
        fs::path outDir = "./~tmp";
        fs::create_directory(outDir);

        fs::path path("D:\\Projects\\marc\\Debug\\stored-packed.mcf");
        marc::FS *cFilePtr = new marc::FS(path, marc::fs_types::writeMode);

        streamList.sort([](const marc::Stream &F, const marc::Stream &S) {
            return F.getOffset() < S.getOffset();
        });

        MarcHeader Header;
        std::memcpy(Header.Signature, marcSignature, sizeof(marcSignature));
        std::memcpy(Header.Version, marcVersion, sizeof(marcVersion));
        Header.OriginalSize = filePtr->getFileSize();
        Header.OriginalCRC32 = Utils::calculateCrc32(filePtr, 0, filePtr->getFileSize());
        Header.FirstCompressedStreamOffset = -1;

        // Keep bytes for header
        cFilePtr->seek(sizeof(MarcHeader), marc::fs_types::fileBegin);

        for (auto stream : streamList) {
            std::string fileName = std::to_string(stream.getOffset()) + ".wav";
            fs::path outPath = outDir / fileName;
            Utils::extractFileFromStream(filePtr, stream.getOffset(), stream.getSize(), outPath.u8string());
            // TakCompress(outPath, outPath.replace_extension(".tak"), 9);
        }

        // TODO
        /*uint firstOffset = (uint)streamList.front().getOffset();
        if (firstOffset > 0) {
            // TODO: Rewrite (make chunk read)
            std::cout << firstOffset << std::endl;

            char *buffer = new char[firstOffset];
            filePtr->seek(0, marc::fs_types::fileBegin);
            filePtr->read(firstOffset, buffer);
            cFilePtr->write(firstOffset, buffer);
        }*/

        cFilePtr->close();
       // fs::remove(outDir);

        /*for (auto stream : streamList) {
            std::cout << "Stream: " << stream.getType()
                << ", Size: " << stream.getSize()
                << ", Offset: " << stream.getOffset()
                << std::endl;
        }*/
    }

    /*bool StreamCompressor::WavpackCompress(fs::path inputFile, fs::path outputFile, unsigned short level) {
#if _WIN64
        bp::child process(bp::search_path("packers/wavpack_x64.exe"), "-h", inputFile, outputFile);
#else
        bp::child process(bp::search_path("packers/wavpack_x32.exe"), "-h", inputFile, outputFile);
#endif
        return process.exit_code() == 0;
    }

    bool StreamCompressor::TakCompress(fs::path inputFile, fs::path outputFile, unsigned short level) {
        bp::ipstream Pipe;
        bp::child process(bp::search_path("packers/tak.exe"), "-e", "-overwrite", "-wm0", "-tn4", "-p4m", inputFile, outputFile);
        process.wait();
        return process.exit_code() == 0;
    }*/
}