#include <iostream>
#include <string>
#include <omp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <glm/glm.hpp>

#include "pointcloud.h"
#include "utilityCore.hpp"
#include "icp.h"

namespace pt = boost::property_tree;


int main(int argc, char** argv) {

    std::string scene_file;
    std::string target_file;
    int num_threads = 4;
    int num_iterations = 10;
    std::string err_file = "error_history.csv";
    std::string out_dir = "output";
    int save_interval = 10;

    std::string config_file = "sample_config.xml";

    if (argc > 1) {
        config_file = argv[1];
    } else {
        std::cout << "[error] please provide path to config file.\n";
        std::cout << "usage: ./icp <config_file>\n";
        return -1;
    }

    pt::ptree config;
    std::ifstream file(config_file);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        pt::read_xml(buffer, config);
    }

    scene_file = config.get<std::string>("icp.scene");
    target_file = config.get<std::string>("icp.target");
    num_threads = config.get<int>("icp.numThreads");
    num_iterations = config.get<int>("icp.numIterations");
    err_file = config.get<std::string>("icp.errorFile");
    out_dir = config.get<std::string>("icp.outDir");
    save_interval = config.get<int>("icp.saveInterval");

    // Creating a directory
    if (mkdir(out_dir.c_str(), 0777) == -1)
        std::cerr << "Error: " << strerror(errno) << "\n";
    else
        std::cout << "Directory created for storing outputs: " << out_dir << "\n";

    Pointcloud *scene = new Pointcloud(scene_file);
    Pointcloud *target = new Pointcloud(target_file);

    glm::vec3 t(
        config.get<float>("icp.initialTransform.translate.x"),
        config.get<float>("icp.initialTransform.translate.y"),
        config.get<float>("icp.initialTransform.translate.z")
    );
    glm::vec3 r(
        config.get<float>("icp.initialTransform.rotate.x"),
        config.get<float>("icp.initialTransform.rotate.y"),
        config.get<float>("icp.initialTransform.rotate.z")
    );
    glm::vec3 s(1, 1, 1);
    glm::mat4 initial_transform = utilityCore::buildTransformationMatrix(t, r, s);

    double start_time = omp_get_wtime();

    ICP::init(scene->points, target->points, out_dir, save_interval, num_threads, initial_transform);

    while (num_iterations--)
        ICP::step();

    ICP::end();
    ICP::write_history(err_file);

    double total_time = omp_get_wtime() - start_time;
    printf("total_time: %f\n", total_time);

    return 0;
}