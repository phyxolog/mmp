#include "pch.h"
#include <iostream>

#include "FS.hpp"


int main() {
    marc::FS fs("D:\\Projects\\marc\\Debug\\stored.zip", marc::fs_types::readMode, marc::fs_types::SequentialScan);
    char *buffer = new char[20];
    //fs.seek(16179869184);
    fs.read(2, buffer);
    std::cout << buffer[0] << std::endl;

    return 0;
}