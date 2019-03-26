#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ICP {
    void initICP(std::vector<glm::vec4> scene, std::vector<glm::vec4> target);
    void endICP();
}