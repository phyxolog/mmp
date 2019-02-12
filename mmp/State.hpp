#pragma once

#ifndef STATE_HPP
#define STATE_HPP

#include <experimental/filesystem>
#include <string>
#include <map>

namespace mmp {
    namespace fs = std::experimental::filesystem;

    class State {
    public:
        // Global variables
        fs::path inputFile;
        fs::path outputFile;
        fs::path outputDir;
        char command;
        bool verbose;

        // With default paramenets
        unsigned int bufferSize = 1 * 1024 * 1024; // 1 Mb
        std::map<std::string, bool> analyzerOptions = {
            { "wav", true },
        };

        std::map<std::string, unsigned int> compressorOptions = {
            { "tak", 9 },
        };

        static State *instance();

    private:
        State() { };
        State(const State &root) = delete;
        State& operator=(const State&) = delete;

        static State *_instance;
    };
}

#endif // STATE_HPP