#ifndef RAYTRACING_UTILS_H
#define RAYTRACING_UTILS_H

#include "glm/glm.hpp"
#include "enable_stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "filesystem.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}


inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}

//glm::vec3 reflect(const glm::vec3 &light_dir, const glm::vec3 &normal) {
//    return light_dir - 2 * glm::dot(normal, light_dir) * normal;
//}

static glm::vec4 veclerp(const glm::vec4 &v1, const glm::vec4 &v2, float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}

static glm::vec3 veclerp(const glm::vec3 &v1, const glm::vec3 &v2, float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}

static glm::vec2 veclerp(const glm::vec2 &v1, const glm::vec2 &v2, float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}

static float interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3,
                         const float num1, const float num2, const float num3, float weight) {
    return (alpha * num1 / z1 + beta * num2 / z2 + gamma * num3 / z3) * weight;
}

static glm::vec2 interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3,
                             const glm::vec2 &vert1, const glm::vec2 &vert2, const glm::vec2 &vert3, float weight) {
    return (alpha * vert1 / z1 + beta * vert2 / z2 + gamma * vert3 / z3) * weight;
}

static glm::vec3 interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3,
                             const glm::vec3 &vert1, const glm::vec3 &vert2, const glm::vec3 &vert3, float weight) {
    return (alpha * vert1 / z1 + beta * vert2 / z2 + gamma * vert3 / z3) * weight;
}

static glm::vec4 interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3,
                             const glm::vec4 &vert1, const glm::vec4 &vert2, const glm::vec4 &vert3, float weight) {
    return (alpha * vert1 / z1 + beta * vert2 / z2 + gamma * vert3 / z3) * weight;
}


#endif //RAYTRACING_UTILS_H
