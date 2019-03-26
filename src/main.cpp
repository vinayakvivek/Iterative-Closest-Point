#include <iostream>
#include <string>

#include "pointcloud.h"
#include "icp.h"


int main(int argc, char** argv) {

    std::string scene_file;
    int num_threads = 4;

    if (argc > 1) {
        scene_file = argv[1];
    } else {
        std::cout << "[error] please provide path to scene file.\n";
        std::cout << "usage: ./icp <scene_file>\n";
        return -1;
    }

    if (argc > 2) {
        num_threads = atoi(argv[2]);
    }

    std::cout << scene_file << "\n";

    Pointcloud *scene = new Pointcloud(scene_file);
    Pointcloud *target = new Pointcloud(scene_file);

    ICP::init(scene->points, target->points, num_threads);
    ICP::step();
    ICP::end();

    return 0;
}