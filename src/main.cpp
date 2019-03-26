#include <iostream>
#include <string>

#include "pointcloud.h"
#include "icp.h"


int main(int argc, char** argv) {

    std::string scene_file;

    if (argc > 1) {
        scene_file = argv[1];
    } else {
        std::cout << "[error] please provide path to scene file.\n";
        std::cout << "usage: ./icp <scene_file>\n";
        return -1;
    }

    std::cout << scene_file << "\n";

    Pointcloud *scene = new Pointcloud(scene_file);
    Pointcloud *target = new Pointcloud(scene_file);

    ICP::initICP(scene->points, target->points);

    return 0;
}