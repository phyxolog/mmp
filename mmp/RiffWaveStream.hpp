#pragma once

#include "BaseStream.hpp"

namespace mmp {
    class RiffWaveStream : public BaseStream {
    public:
        std::string  getFileExtension() {
            return "wav";
        }

        std::string  getTypeAsString() {
            return "RIFF WAVE";
        }

        std::string  getTypeAsShortString() {
            return RIFF_WAVE_SHORT_TYPE;
        }
    };
}