#include "icp.h"
#include "utilityCore.hpp"
#include <omp.h>

#define INITIAL_TRANSFORM 1

int size_scene;
int size_target;
int num_points;

glm::vec4 *pos;
int *dist;
int *pair;

void transform_points(int n, glm::vec4* points, glm::mat4 transformation) {
    #pragma omp parallel for default(none) shared(n, points, transformation)
    for (int i = 0; i < n; ++i) {
        points[i] = transformation * points[i];
    }
}

/*
 * basic setup,
 */
void ICP::initICP(std::vector<glm::vec4> scene, std::vector<glm::vec4> target) {

    omp_set_num_threads(4);

    size_scene = scene.size();
    size_target = target.size();
    num_points = size_scene + size_target;

    pos = (glm::vec4*) malloc(num_points * sizeof(glm::vec4));
    std::memcpy(pos, &scene[0], size_scene * sizeof(glm::vec4));
    std::memcpy(&pos[size_scene], &target[0], size_target * sizeof(glm::vec4));

    pair = (int*) malloc(size_target * sizeof(int));

    #if INITIAL_TRANSFORM
    {
        glm::vec3 t(80, -22, 100);
        glm::vec3 r(-.5, .6, .8);
        glm::vec3 s(1, 1, 1);
        glm::mat4 initial_transform = utilityCore::buildTransformationMatrix(t, r, s);
        // utilityCore::printMat4(initial_transform);

        transform_points(size_target, &pos[size_scene], initial_transform);
    }
    #endif

    for (int i = 0; i < num_points; ++i) {
        utilityCore::printVec4(pos[i]);
    }
}