#pragma once

#include <list>

#include "TurboBM.hpp"
#include "Types.hpp"
#include "Stream.hpp"
#include "IStreamDetector.hpp"

namespace marc {
    class RiffWaveDetector : public IStreamDetector {
    private:
        const char* signature;
        uint signatureSize;
        marc::skiptableType skiptableType;
        marc::occtableType occtableType;

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

        bool validHeader(const char *buffer, uint index) {
            return std::memcmp(buffer + index, "RIFF", 4) == 0 && std::memcmp(buffer + index + 8, "WAVE", 4) == 0;
        }

    public:
        RiffWaveDetector() {
            signature = "RIFF";
            signatureSize = std::strlen(signature);
            skiptableType = marc::TurboBM::CreateSkipTable(signature, signatureSize);
            occtableType = marc::TurboBM::CreateOccTable(signature, signatureSize);
        }

        void execute(const char *buffer, uint bufferSize, int64 currentOffset, std::list<Stream> &streamList) {
            Stream stream;
            uint index = marc::TurboBM::Search(buffer, bufferSize, occtableType, skiptableType, signature, signatureSize);

            while (index != -1) {
                if (validHeader(buffer, index)) {
                    stream.setOffset(currentOffset + index);
                    stream.setType(RIFF_WAVE_TYPE);
                    streamList.push_front(stream);
                }

                index = marc::TurboBM::Search(buffer, bufferSize, occtableType, skiptableType, signature, signatureSize, index + 1);
            }
        }

        void analyze(FS* filePtr, std::list<Stream> &streamList) {
            Header *header = new Header;

            for (auto stream = streamList.rbegin(); stream != streamList.rend(); stream++) {
                filePtr->seek(stream->getOffset());
                filePtr->read(sizeof(Header), header);
                
                stream->setSize(header->ChunkSize + 8);

                // TODO: Analyze stream on compressible/validable
            }
        }
    };
}