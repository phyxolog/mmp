#pragma once

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

        static State *instance() {
            if (!_instance) {
                _instance = new State();
            }

            return _instance;
        }

    private:
        State() { };
        State(const State &root) = delete;
        State& operator=(const State&) = delete;

        static State *_instance;
    };

    State *State::_instance = 0;
}