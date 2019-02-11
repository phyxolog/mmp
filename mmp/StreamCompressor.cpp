#include "pch.hpp"
#include "StreamCompressor.hpp"

namespace mmp {
    namespace fs = std::experimental::filesystem;

    StreamCompressor::StreamCompressor(mmp::FS *filePtr, mmp::FS *cFilePtr, uint bufferSize)
        : filePtr(filePtr), cFilePtr(cFilePtr), bufferSize(bufferSize) { }
    StreamCompressor::~StreamCompressor() { }

    void StreamCompressor::execute(std::list<Stream> &streamList) {
        streamList.sort([](const mmp::Stream &F, const mmp::Stream &S) {
            return F.getOffset() < S.getOffset();
        });

        MarcHeader Header;
        std::memcpy(Header.signature, mmp::Signature, sizeof(mmp::Signature));
        std::memcpy(Header.version, mmp::Version, sizeof(mmp::Version));
        Header.crc32 = Utils::calculateCrc32(filePtr, 0, filePtr->getFileSize());
        Header.firstCompressedStreamOffset = -1;

        // Keep bytes for header
        cFilePtr->seek(sizeof(MarcHeader), mmp::fs_types::fileBegin);

        MarcCompressedStream compressedStream;
        std::list<MarcCompressedStream> compressedStreamList;
        int64 prevOffset = 0, savedBytes = 0, fileSize = filePtr->getFileSize();

        // TODO: Filter by type
        // TOOD: Choose compressor
        // TODO: Universal compressor
        // TODO: Choose level
        for (auto stream : streamList) {
            // Write non-compressed data
            if (stream.getOffset() > prevOffset) {
                Utils::CopyData(
                    filePtr,
                    cFilePtr,
                    prevOffset,
                    stream.getOffset() - prevOffset
                );
            }

            fs::path outCompressedFileName;
            CompressStream(stream, compressedStream, outCompressedFileName);

            if (compressedStream.compressedSize >= stream.getSize()) {
                Utils::CopyData(filePtr, cFilePtr, stream.getOffset(), stream.getSize());
                continue;
            }

            std::cout << "c_Tak: " << stream.getSize() << " --> " << compressedStream.compressedSize << std::endl;

            compressedStream.type = stream.getType();
            compressedStream.compressedOffset = cFilePtr->seek(0, fs_types::fileCurrent);
            compressedStream.originalOffset = stream.getOffset();
            compressedStream.originalSize = stream.getSize();
            compressedStream.originalCRC32 =
                Utils::calculateCrc32(filePtr, stream.getOffset(), stream.getSize());

            compressedStreamList.push_front(compressedStream);

            cFilePtr->seek(sizeof(MarcInternalCompressedStream), fs_types::fileCurrent);
            mmp::FS *outCompressedFile = new mmp::FS(outCompressedFileName, mmp::fs_types::readMode);

            Utils::CopyData(outCompressedFile, cFilePtr, 0, compressedStream.compressedSize);

            outCompressedFile->close();
            fs::remove(outCompressedFileName);

            savedBytes += stream.getSize() - compressedStream.compressedSize;
            prevOffset = stream.getOffset() + stream.getSize();
        }

        for (auto streamIterator = compressedStreamList.rbegin(); streamIterator != compressedStreamList.rend(); streamIterator++) {
            MarcInternalCompressedStream internalCompresseedStream;
            internalCompresseedStream.compressedSize = streamIterator->compressedSize;
            internalCompresseedStream.compressor = streamIterator->compressor;
            internalCompresseedStream.originalCRC32 = streamIterator->originalCRC32;
            internalCompresseedStream.originalOffset = streamIterator->originalOffset;
            internalCompresseedStream.type = streamIterator->type;

            auto nextStreamIterator = std::next(streamIterator, 1);
            if (nextStreamIterator != compressedStreamList.rend()) {
                internalCompresseedStream.nextCompressedStreamOffset = nextStreamIterator->compressedOffset;
            } else {
                internalCompresseedStream.nextCompressedStreamOffset = -1;
            }

            cFilePtr->seek(streamIterator->compressedOffset, fs_types::fileBegin);
            cFilePtr->write(sizeof(MarcInternalCompressedStream), &(*streamIterator));
        }

        // Write other non-compressed data
        if (prevOffset < fileSize) {
            Utils::CopyData(
                filePtr,
                cFilePtr,
                prevOffset,
                fileSize - prevOffset
            );
        }

        // If we compress some data,
        // we need set pointer to first compressed stream
        if (savedBytes != 0) {
            Header.firstCompressedStreamOffset =
                sizeof(MarcHeader) + streamList.front().getOffset();
        }

        // Write header
        cFilePtr->seek(0, fs_types::fileBegin);
        cFilePtr->write(sizeof(MarcHeader), &Header);
    }

    void StreamCompressor::CompressStream(Stream &stream, MarcCompressedStream &compressedStream, fs::path &outputFile) {
        std::string outFileName = std::to_string(stream.getOffset())
            + "_" + std::to_string(stream.getSize())
            + "." + mmp::StreamExts[stream.getType()];
        fs::path outFile = fs::absolute(outFileName);
        Utils::extractFileFromStream(filePtr, stream.getOffset(), stream.getSize(), outFile.string());
        bool result = false;

        switch (stream.getType()) {
        case RIFF_WAVE_TYPE:
            result = TakCompress(outFile, outputFile, 9);
            break;
        default:
            break;
        }

        if (result) {
            compressedStream.compressedSize = fs::file_size(outputFile);
        } else {
            compressedStream.compressedSize = stream.getSize();
        }

        fs::remove(outFile);
    }

    bool StreamCompressor::TakCompress(fs::path inputFile, fs::path &outputFile, unsigned short level) {
        std::string _level = "";
        outputFile = fs::path(inputFile).replace_extension(".tak");

        switch (level)
        {
        case 1:
            _level = "-p0";
            break;
        case 2:
            _level = "-p1";
            break;
        case 3:
            _level = "-p1m";
            break;
        case 4:
            _level = "-p2";
            break;
        case 5:
            _level = "-p2m";
            break;
        case 6:
            _level = "-p3";
            break;
        case 7:
            _level = "-p3m";
            break;
        case 8:
            _level = "-p4";
            break;
        case 9:
            _level = "-p4m";
            break;
        default:
            return false;
        }

        std::stringstream args;
        args
            << "-e -overwrite -wm0 -tn4 "
            << _level << " "
            << "\"" << inputFile.string() << "\" "
            << "\"" << outputFile.string() << "\"";

        int result = Utils::execAndWait(fs::absolute(fs::path("packers/tak.exe")).string(), args.str());
        return result == 0;
    }
}
