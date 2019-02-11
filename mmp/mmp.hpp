#pragma once

#include <experimental/filesystem>
#include <list>

#include "StreamAnalyzer.hpp"
#include "RiffWaveDetector.hpp"
#include "StreamCompressor.hpp"

#define DEFAULT_BUFFER_SIZE 1 * 1024 * 1024

static const std::string logo =
    "----------------------------------------------------------\n"
    "|                 [M]ulti[M]edia [P]acker                |\n"
    "|             https://github.com/phyxolog/mmp           |\n"
    "----------------------------------------------------------\n";

static const std::string usageMessage =
    "Usage:\n"
    "    mmp.exe <command> [options] <input_file>\n\n"
    "    Commands:\n"
    "      c - compress input file\n"
    "      s - scan only input file\n"
    "      e - extract found streams from input file\n\n"
    "    Detect options:\n"
    "      --wav=N          - enable RIFF WAVE detect (default: 1)\n\n"
    "    Compress options:\n"
    "      --tak=N          - TAK compression level (0..9) (default: 9)\n\n"
    "    Other options:\n"
    "      --out=<filename> - path to output file name\n"
    "      --outdir=<path>  - path to output folder (for extracted files)\n"
    "      --bufsize=N      - set buffer size (default: 256kb).\n"
    "      --verbose=N      - enable verbose mode (default: 1).\n\n";
