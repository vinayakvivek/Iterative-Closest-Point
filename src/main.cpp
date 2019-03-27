#include <iostream>
#include <string>
#include <omp.h>

#include "pointcloud.h"
#include "icp.h"


int main(int argc, char** argv) {

    std::string scene_file;
    std::string target_file;
    int num_threads = 4;
    int num_iterations = 10;
    std::string out_file = "error_history.csv";

    if (argc > 2) {
        scene_file = argv[1];
        target_file = argv[2];
    } else {
        std::cout << "[error] please provide path to scene file.\n";
        std::cout << "usage: ./icp <scene_file> <target_file> [<num_threads>, <num_iterations>, <out_file>]\n";
        return -1;
    }

    if (argc > 3) {
        num_threads = atoi(argv[3]);
    }

    if (argc > 4) {
        num_iterations = atoi(argv[4]);
    }

    if (argc > 5) {
        out_file = argv[5];
    }

    std::cout << scene_file << "\n";

    Pointcloud *scene = new Pointcloud(scene_file);
    Pointcloud *target = new Pointcloud(target_file);

    double start_time = omp_get_wtime();

    ICP::init(scene->points, target->points, num_threads);

    while (num_iterations--)
        ICP::step();

    ICP::end();
    ICP::write_history(out_file);

    double total_time = omp_get_wtime() - start_time;
    printf("total_time: %f\n", total_time);

    return 0;
}