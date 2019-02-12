#pragma once

#include <list>

#include "Types.hpp"
#include "TurboBM.hpp"
#include "RiffWaveStream.hpp"
#include "IStreamDetector.hpp"

namespace mmp {
    class RiffWaveDetector : public IStreamDetector {
    private:
        const char* signature;
        size_t signatureSize;
        mmp::skiptableType skiptableType;
        mmp::occtableType occtableType;

#pragma pack(push, 1)
        typedef struct Header {
            char ChunkId[4];
            unsigned long ChunkSize;
            char Format[4];
            char Subchunk1Id[4];
            unsigned long Subchunk1Size;
            unsigned short AudioFormat;
            unsigned short NumChannels;
            unsigned long SampleRate;
            unsigned long ByteRate;
            unsigned short BlockAlign;
            unsigned short BitsPerSample;
            char Subchunk2Id[4];
            unsigned long Subchunk2Size;
        } Header;
#pragma pack(pop)

        bool validHeader(const char *buffer, size_t index) {
            return std::memcmp(buffer + index, "RIFF", 4) == 0 && std::memcmp(buffer + index + 8, "WAVE", 4) == 0;
        }

    public:
        RiffWaveDetector() {
            signature = "RIFF";
            signatureSize = std::strlen(signature);
            skiptableType = mmp::TurboBM::createSkipTable(signature, signatureSize);
            occtableType = mmp::TurboBM::createOccTable(signature, signatureSize);
        }

        void execute(const char *buffer, unsigned int bufferSize, uintmax_t currentOffset, std::list<BaseStream*> &streamList) {
            size_t index = mmp::TurboBM::search(buffer, bufferSize, occtableType, skiptableType, signature, signatureSize);

            while (index != -1) {
                if (validHeader(buffer, index)) {
                    RiffWaveStream *stream = new RiffWaveStream();
                    stream->setOffset(currentOffset + index);
                    streamList.push_front(stream);
                }

                index = mmp::TurboBM::search(buffer, bufferSize, occtableType, skiptableType, signature, signatureSize, index + 1);
            }
        }

        void analyze(FileStream* filePtr, std::list<BaseStream*> &streamList) {
            Header *header = new Header;

            for (auto stream = streamList.rbegin(); stream != streamList.rend(); stream++) {
                filePtr->seek((*stream)->getOffset());
                filePtr->read(sizeof(Header), header);
                
                unsigned long realSize = 4 + (8 + header->Subchunk1Size) + (8 + header->Subchunk2Size);

                // [X] Invalid header
                if (realSize != header->ChunkSize) {
                    DiffType dt;

                    dt.newData = ::operator new(4);
                    dt.oldData = ::operator new(4);

                    dt.size = 4;
                    dt.offset = 4;
                    memcpy(dt.newData, &realSize, 4);
                    memcpy(dt.oldData, &header->ChunkSize, 4);
                    (*stream)->addDiff(dt);
                }

                (*stream)->setSize(realSize + 8);

                // TODO: Analyze stream on compressible/validable
            }

            delete header;
        }
    };
}
