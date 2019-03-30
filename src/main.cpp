#include <iostream>
#include <string>
#include <omp.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "pointcloud.h"
#include "icp.h"


int main(int argc, char** argv) {

    std::string scene_file;
    std::string target_file;
    int num_threads = 4;
    int num_iterations = 10;
    std::string err_file = "error_history.csv";
    std::string out_dir = "../output/test";
    int save_interval = 10;

    if (argc > 2) {
        scene_file = argv[1];
        target_file = argv[2];
    } else {
        std::cout << "[error] please provide path to scene file.\n";
        std::cout << "usage: ./icp <scene_file> <target_file> \
            [<num_threads>, <num_iterations>, <out_dir>, <save_interval>]\n";
        return -1;
    }

    if (argc > 3) {
        num_threads = atoi(argv[3]);
    }

    if (argc > 4) {
        num_iterations = atoi(argv[4]);
    }

    if (argc > 5) {
        out_dir = argv[5];
    }

    if (argc > 6) {
        save_interval = atoi(argv[6]);
    }

    // Creating a directory
    if (mkdir(out_dir.c_str(), 0777) == -1)
        std::cerr << "Error: " << strerror(errno) << "\n";
    else
        std::cout << "Directory created for storing outputs: " << out_dir << "\n";

    std::cout << scene_file << "\n";

    Pointcloud *scene = new Pointcloud(scene_file);
    Pointcloud *target = new Pointcloud(target_file);

    double start_time = omp_get_wtime();

    ICP::init(scene->points, target->points, out_dir, save_interval, num_threads);

    while (num_iterations--)
        ICP::step();

    ICP::end();
    ICP::write_history(err_file);

    double total_time = omp_get_wtime() - start_time;
    printf("total_time: %f\n", total_time);

    return 0;
}