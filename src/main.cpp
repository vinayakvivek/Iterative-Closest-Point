#include <iostream>
#include <string>

#include "pointcloud.h"
#include "icp.h"


int main(int argc, char** argv) {

    std::string scene_file;
    int num_threads = 4;
    int num_iterations = 10;
    std::string out_file = "error_history.csv";

    if (argc > 1) {
        scene_file = argv[1];
    } else {
        std::cout << "[error] please provide path to scene file.\n";
        std::cout << "usage: ./icp <scene_file> [<num_threads>, <num_iterations>, <out_file>]\n";
        return -1;
    }

    if (argc > 2) {
        num_threads = atoi(argv[2]);
    }

    if (argc > 3) {
        num_iterations = atoi(argv[3]);
    }

    if (argc > 4) {
        out_file = argv[4];
    }

    std::cout << scene_file << "\n";

    Pointcloud *scene = new Pointcloud(scene_file);
    Pointcloud *target = new Pointcloud(scene_file);

    ICP::init(scene->points, target->points, num_threads);

    while (num_iterations--)
        ICP::step();

    ICP::end();
    ICP::write_history(out_file);

    return 0;
}