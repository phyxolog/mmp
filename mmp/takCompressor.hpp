#pragma once

#include "IExternalCompressor.hpp"
#include "Utils.hpp"

namespace mmp {
    class TakCompressor : public IExternalCompressor {
    private:
        fs::path inputFile;
        fs::path outputFile;
        unsigned short level;
    public:
        TakCompressor(fs::path inputFile, fs::path outputFile, unsigned short level)
            : inputFile(inputFile), outputFile(outputFile), level(level) { };

        std::string getType() { return "Tak"; }

        bool execute(fs::path &_outputFile) {
            std::string _level = "";
            _outputFile = outputFile = fs::path(inputFile).replace_extension(".tak");

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
    };
}