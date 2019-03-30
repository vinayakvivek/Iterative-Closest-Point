#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>


namespace ICP {
    void init(
        std::vector<glm::vec4> scene,
        std::vector<glm::vec4> target,
        std::string out_dir,
        int save_interval,
        int num_threads);
    void end();

    void step();
    void write_history(std::string out_filename);
}