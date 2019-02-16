#pragma once

#include <experimental/filesystem>
#include <string>

namespace mmp {
    namespace fs = std::experimental::filesystem;

    class IExternalCompressor {
    public:
        IExternalCompressor() { };
        virtual std::string getType() { return "unknown"; }
        virtual bool execute(fs::path&) { return false; };
    };
}