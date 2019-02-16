#include "pch.hpp"
#include "StreamCompressor.hpp"

namespace mmp {
    namespace fs = std::experimental::filesystem;

    StreamCompressor::StreamCompressor(mmp::FileStream *filePtr, mmp::FileStream *cFilePtr, unsigned int bufferSize)
        : filePtr(filePtr), cFilePtr(cFilePtr), bufferSize(bufferSize) { }
    StreamCompressor::~StreamCompressor() { }

    void StreamCompressor::execute(std::list<BaseStream*> &streamList) {
        streamList.sort([](const BaseStream *F, const BaseStream *S) {
            return F->getOffset() < S->getOffset();
        });

        PackerHeader Header;
        std::memcpy(Header.signature, mmp::Signature, sizeof(mmp::Signature));
        std::memcpy(Header.version, mmp::Version, sizeof(mmp::Version));
        Header.crc32 = Utils::calculateCrc32(filePtr, 0, filePtr->getFileSize());
        Header.firstCompressedStreamOffset = -1;

        // Keep bytes for header
        cFilePtr->seek(sizeof(PackerHeader), mmp::fs_types::fileBegin);

        CompressedStream compressedStream;
        std::list<CompressedStream> compressedStreamList;
        uintmax_t prevOffset = 0, savedBytes = 0, fileSize = filePtr->getFileSize();

        for (auto stream : streamList) {
            // Write non-compressed data
            if ((*stream).getOffset() > prevOffset) {
                Utils::copyData(filePtr, cFilePtr, prevOffset, (*stream).getOffset() - prevOffset);
            }

            fs::path outCompressedFileName;
            compressStream((*stream), compressedStream, outCompressedFileName);

            if (compressedStream.compressedSize >= (*stream).getSize()) {
                Utils::copyData(filePtr, cFilePtr, (*stream).getOffset(), (*stream).getSize());
                prevOffset = (*stream).getOffset() + (*stream).getSize();
                continue;
            }

            std::cout << "c_" << compressedStream.compressorType << ": " << (*stream).getSize() << " --> " << compressedStream.compressedSize << std::endl;

            std::memcpy(compressedStream.metaData.type, (*stream).getTypeAsShortString().c_str(), (*stream).getTypeAsShortString().length());
            compressedStream.compressedOffset = cFilePtr->seek(0, fs_types::fileCurrent);
            compressedStream.originalOffset = (*stream).getOffset();
            compressedStream.originalSize = (*stream).getSize();
            compressedStream.originalCRC32 =
                Utils::calculateCrc32(filePtr, (*stream).getOffset(), (*stream).getSize());

            compressedStreamList.push_front(compressedStream);

            cFilePtr->seek(sizeof(InternalCompressedStream), fs_types::fileCurrent);
            mmp::FileStream *outCompressedFile = new mmp::FileStream(outCompressedFileName, mmp::fs_types::readMode);

            Utils::copyData(outCompressedFile, cFilePtr, 0, compressedStream.compressedSize);

            outCompressedFile->close();
            fs::remove(outCompressedFileName);

            savedBytes += (*stream).getSize() - compressedStream.compressedSize;
            prevOffset = (*stream).getOffset() + (*stream).getSize();
        }

        for (auto streamIterator = compressedStreamList.rbegin(); streamIterator != compressedStreamList.rend(); streamIterator++) {
            if (streamIterator->compressedSize >= streamIterator->originalSize) {
                continue;
            }

            InternalCompressedStream internalCompresseedStream;
            internalCompresseedStream.compressedSize = streamIterator->compressedSize;
            std::memcpy(internalCompresseedStream.metaData.type, streamIterator->metaData.type, std::strlen(streamIterator->metaData.type));
            std::memcpy(internalCompresseedStream.metaData.compressor, streamIterator->metaData.compressor, std::strlen(streamIterator->metaData.compressor));
            internalCompresseedStream.originalCRC32 = streamIterator->originalCRC32;
            internalCompresseedStream.originalOffset = streamIterator->originalOffset;

            auto nextStreamIterator = std::next(streamIterator, 1);
            if (nextStreamIterator != compressedStreamList.rend()) {
                internalCompresseedStream.nextCompressedStreamOffset = nextStreamIterator->compressedOffset;
            } else {
                internalCompresseedStream.nextCompressedStreamOffset = -1;
            }

            cFilePtr->seek(streamIterator->compressedOffset, fs_types::fileBegin);
            cFilePtr->write(sizeof(InternalCompressedStream), &(*streamIterator));
        }

        // Write other non-compressed data
        if (prevOffset < fileSize) {
            Utils::copyData(filePtr, cFilePtr, prevOffset, fileSize - prevOffset);
        }

        // If we compress some data,
        // we need set pointer to first compressed stream
        if (savedBytes != 0) {
            Header.firstCompressedStreamOffset = sizeof(PackerHeader) + streamList.front()->getOffset();
        }

        // Write header
        cFilePtr->seek(0, fs_types::fileBegin);
        cFilePtr->write(sizeof(PackerHeader), &Header);
    }

    void StreamCompressor::compressStream(BaseStream &stream, CompressedStream &compressedStream, fs::path &outputFile) {
        std::string outFileName = std::to_string(stream.getOffset())
            + "_" + std::to_string(stream.getSize())
            + "." + stream.getFileExtension();
        fs::path outFile = fs::absolute(outFileName);
        Utils::extractFileFromStream(filePtr, stream.getOffset(), stream.getSize(), outFile.string());
        bool result = false;

        std::list<DiffType> diffList = stream.getDiffList();

        if (!diffList.empty()) {
            mmp::FileStream *outFilePtr = new mmp::FileStream(outFile, mmp::fs_types::appendMode);

            for (auto diff : diffList) {
                outFilePtr->seek(diff.offset, mmp::fs_types::fileBegin);
                outFilePtr->write(diff.size, diff.newData);
            }

            outFilePtr->close();
        }

        IExternalCompressor *compressor = new IExternalCompressor();

        if (stream.getTypeAsShortString() == RIFF_WAVE_SHORT_TYPE
            && State::instance()->analyzerOptions["wav"]) {
            if (State::instance()->compressorOptions["tak"] > 0) {
                compressor = new TakCompressor(outFile, outputFile, State::instance()->compressorOptions["tak"]);
            } else if (State::instance()->compressorOptions["frog"] > 0) {
                compressor = new FrogCompressor(outFile, outputFile, State::instance()->compressorOptions["frog"]);
            }
        }

        result = compressor->execute(outputFile);
        compressedStream.compressorType = compressor->getType();

        if (result) {
            compressedStream.compressedSize = fs::file_size(outputFile);
        } else {
            compressedStream.compressedSize = stream.getSize();
        }

        fs::remove(outFile);
    }
}
