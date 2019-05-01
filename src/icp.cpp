#include "icp.h"
#include "utilityCore.hpp"
#include "svd3.h"
#include <omp.h>

#define INITIAL_TRANSFORM 1
#define BLOCK_SIZE 512

int size_scene;
int size_target;
int num_points;

glm::vec4 *pos;
int *pair;
glm::mat4 final_transform;

int curr_step;
int save_interval;
std::string out_dir;
std::string original_file;
std::string transformed_file;
std::string step_file;
std::vector<float> error_history;


__global__ void Transform_cuda(int n, glm::vec4* points, const glm::mat4& transform) {

    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i<n)
    {
        points[i] = transform * points[i];
    }
}

void transform_points(int n, glm::vec4* points, const glm::mat4& transform) {
    #pragma omp parallel for default(none) shared(n, points, transform)
    for (int i = 0; i < n; ++i) {
        points[i] = transform * points[i];
    }
}

float calculate_error() {
    float error = 0.0;
    #pragma omp parallel for \
                default(none) \
                shared(size_target, size_scene, pos, pair) \
                reduction(+: error)
    for (int i = 0; i < size_target; ++i) {
        error += glm::distance2(glm::vec3(pos[i + size_scene]), glm::vec3(pos[pair[i]]));
    }
    error /= size_target;
    return error;
}

void save_points(std::string file_name, glm::vec4* points, int n) {
    std::ofstream f_out;
    f_out.open(file_name);
    if (!f_out.is_open()) {
        std::cout << "Error reading from file - aborting!" << std::endl;
        throw;
    }
    for (int i = 0; i < n; ++i) {
        f_out << points[i].x << " " << points[i].y << " " << points[i].z << "\n";
    }
    f_out.close();
}

#pragma omp declare reduction(glm_vec3_add: glm::vec3: \
    omp_out += omp_in)

/*
 * basic setup,
 */
void ICP::init(
        std::vector<glm::vec4> scene,
        std::vector<glm::vec4> target,
        std::string _out_dir,
        int _save_interval,
        int num_threads,
        glm::mat4 initial_transform) {

    printf("Initializing ICP..\n");

    omp_set_num_threads(num_threads);
    printf("number of threads: %d\n", num_threads);

    size_scene = scene.size();
    size_target = target.size();
    num_points = size_scene + size_target;

    pos = (glm::vec4*) malloc(num_points * sizeof(glm::vec4));
    std::memcpy(pos, &scene[0], size_scene * sizeof(glm::vec4));
    std::memcpy(&pos[size_scene], &target[0], size_target * sizeof(glm::vec4));

    pair = (int*) malloc(size_target * sizeof(int));

    
    Transform_cuda<<<size_target/BLOCK_SIZE,BLOCK_SIZE>>>(size_target, &pos[size_scene], initial_transform);

    //transform_points(size_target, &pos[size_scene], initial_transform);

    final_transform = glm::mat4(1.0f);
    curr_step = 0;
    error_history = std::vector<float>(0);
    save_interval = _save_interval;

    out_dir = _out_dir;
    original_file = _out_dir + "/original.txt";
    transformed_file = _out_dir + "/transformed.txt";
    step_file = _out_dir + "/step_";

    save_points(original_file, pos, size_scene);
    save_points(transformed_file, &pos[size_scene], size_target);
}

void ICP::end() {

    std::string save_path = out_dir + "/final.txt";
    save_points(save_path, &pos[size_scene], size_target);

    free(pos);
    free(pair);
}

void ICP::step() {
    // find the closest point in the scene for each point in the target
    double start_time = omp_get_wtime();

    #pragma omp parallel for default(none) shared(size_scene, size_target, pos, pair)
    for (int i = 0; i < size_target; i++) {
        float best = glm::distance(glm::vec3(pos[0]), glm::vec3(pos[i + size_scene]));
        pair[i] = 0;

        for (int j = 1; j < size_scene; j++) {
            float d = glm::distance(glm::vec3(pos[j]), glm::vec3(pos[i + size_scene]));

            if (d < best) {
                pair[i] = j;
                best = d;
            }
        }
    }

    // Calculate mean centered correspondenses
    glm::vec3 mu_tar(0, 0, 0), mu_cor(0, 0, 0);
    std::vector<glm::vec3> tar_c(size_target);
    std::vector<glm::vec3> cor_c(size_target);

    /*
    #pragma omp parallel for \
                default(none) \
                shared(size_target, size_scene, pos, pair) \
                reduction(glm_vec3_add : mu_tar, mu_cor) */
    for (int i = 0; i < size_target; i++) {
        mu_tar += glm::vec3(pos[i + size_scene]);
        mu_cor += glm::vec3(pos[pair[i]]);
    }

    mu_tar /= size_target;
    mu_cor /= size_target;

    #pragma omp parallel for \
                default(none) \
                shared(size_target, size_scene, pos, pair, tar_c, cor_c, mu_tar, mu_cor)
    for (int i = 0; i < size_target; i++) {
        tar_c[i] = glm::vec3(pos[i + size_scene]) - mu_tar;
        cor_c[i] = glm::vec3(pos[pair[i]]) - mu_cor;
    }

    // Calculate W
    float W[3][3] = {0};

    for (int i = 0; i < size_target; i++) {
        W[0][0] += tar_c[i].x * cor_c[i].x;
        W[0][1] += tar_c[i].y * cor_c[i].x;
        W[0][2] += tar_c[i].z * cor_c[i].x;
        W[1][0] += tar_c[i].x * cor_c[i].y;
        W[1][1] += tar_c[i].y * cor_c[i].y;
        W[1][2] += tar_c[i].z * cor_c[i].y;
        W[2][0] += tar_c[i].x * cor_c[i].z;
        W[2][1] += tar_c[i].y * cor_c[i].z;
        W[2][2] += tar_c[i].z * cor_c[i].z;
    }

    // calculate SVD of W
    float U[3][3] = { 0 };
    float S[3][3] = { 0 };
    float V[3][3] = { 0 };

    svd(
        W[0][0], W[0][1], W[0][2], W[1][0], W[1][1], W[1][2], W[2][0], W[2][1], W[2][2],
        U[0][0], U[0][1], U[0][2], U[1][0], U[1][1], U[1][2], U[2][0], U[2][1], U[2][2],
        S[0][0], S[0][1], S[0][2], S[1][0], S[1][1], S[1][2], S[2][0], S[2][1], S[2][2],
        V[0][0], V[0][1], V[0][2], V[1][0], V[1][1], V[1][2], V[2][0], V[2][1], V[2][2]
    );


    glm::mat3 g_U(glm::vec3(U[0][0], U[1][0], U[2][0]), glm::vec3(U[0][1], U[1][1], U[2][1]), glm::vec3(U[0][2], U[1][2], U[2][2]));
    glm::mat3 g_Vt(glm::vec3(V[0][0], V[0][1], V[0][2]), glm::vec3(V[1][0], V[1][1], V[1][2]), glm::vec3(V[2][0], V[2][1], V[2][2]));

    // Get transformation from SVD
    glm::mat3 R = g_U * g_Vt;
    glm::vec3 t = mu_cor - R * mu_tar;
    glm::mat4 curr_transform = glm::translate(glm::mat4(), t) * glm::mat4(R);

    final_transform = curr_transform * final_transform;

    Transform_cuda<<<size_target/BLOCK_SIZE,BLOCK_SIZE>>>(size_target, &pos[size_scene], curr_transform);

    //transform_points(size_target, &pos[size_scene], curr_transform);
    float error = calculate_error();

    double step_time = omp_get_wtime() - start_time;

    if (curr_step % save_interval == 0) {
        std::string save_path = step_file + std::to_string(curr_step) + ".txt";
        save_points(save_path, &pos[size_scene], size_target);
    }

    printf("[step %d] step_time: %f, error: %f\n", ++curr_step, step_time, error);

    error_history.push_back(error);
}

void ICP::write_history(std::string out_filename) {
    std::ofstream out_file;
    out_file.open(out_filename);

    out_file << "step,error\n";
    for (int i = 1; i <= error_history.size(); ++i) {
        out_file << i << "," << error_history[i - 1] << "\n";
    }
    out_file.close();
}
