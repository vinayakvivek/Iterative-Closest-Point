#include <iostream>
#include <string>

#include "pointcloud.h"


int main(int argc, char** argv) {

    std::string scene_file;

    if (argc > 1) {
        scene_file = argv[1];
    } else {
        std::cout << "[error] please provide path to scene file.\n";
        return -1;
    }

    std::cout << scene_file << "\n";
    return 0;
}