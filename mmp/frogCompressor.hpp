#pragma once
#pragma once

#include "IExternalCompressor.hpp"
#include "Utils.hpp"

namespace mmp {
    class FrogCompressor : public IExternalCompressor {
    private:
        fs::path inputFile;
        fs::path outputFile;
        unsigned short level;
    public:
        FrogCompressor(fs::path inputFile, fs::path outputFile, unsigned short level)
            : inputFile(inputFile), outputFile(outputFile), level(level) { };

        std::string getType() { return "Frog"; }

        bool execute(fs::path &_outputFile) {
            std::string _level = "";
            _outputFile = outputFile = fs::path(inputFile).replace_extension(".ofr");

            switch (level)
            {
            case 1:
                _level = "--mode fast --experimental --seek min --optimize best";
                break;
            case 2:
                _level = "--mode normal --experimental --seek min --optimize best";
                break;
            case 3:
                _level = "--mode high --experimental --seek min --optimize best";
                break;
            case 4:
                _level = "--mode extra --experimental --seek min --optimize best";
                break;
            case 5:
                _level = "--mode best --experimental --seek min --optimize best";
                break;
            case 6:
                _level = "--mode highnew --experimental --seek min --optimize best";
                break;
            case 7:
                _level = "--mode extranew --experimental --seek min --optimize best";
                break;
            case 8:
                _level = "--mode bestnew --seek min --optimize best";
                break;
            case 9:
                _level = "--maximumcompression --experimental --seek min --optimize best";
                break;
            default:
                return false;
            }

            std::stringstream args;
            args
                << "--encode --overwrite "
                << _level << " "
                << "\"" << inputFile.string() << "\" "
                << "--output \"" << outputFile.string() << "\"";

            int result = Utils::execAndWait(fs::absolute(fs::path("packers/frog.exe")).string(), args.str());
            return result == 0;
        }
    };
}