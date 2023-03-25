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

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
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

//void set_pixel(unsigned char *pixel_data, glm::vec3 pixel_color, int samples_per_pixel = 1) {
//    auto r = pixel_color.x;
//    auto g = pixel_color.y;
//    auto b = pixel_color.z;
//
//
//    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
//    auto scale = 1.0f / samples_per_pixel;
//    r = std::pow(scale * r, 1.0f / 2.2f);
//    g = std::pow(scale * g, 1.0f / 2.2f);
//    b = std::pow(scale * b, 1.0f / 2.2f);
//
//    *pixel_data++ = static_cast<unsigned char>(std::clamp(r * 255.0, 0.0, 255.0));
//    *pixel_data++ = static_cast<unsigned char>(std::clamp(g * 255.0, 0.0, 255.0));
//    *pixel_data++ = static_cast<unsigned char>(std::clamp(b * 255.0, 0.0, 255.0));
//}


void loadTexture(const char *path) {
    int image_width, image_height, image_channel = 3;
    unsigned char *data = stbi_load(FileSystem::getPath("textures/earthmap.jpg").c_str(), &image_width, &image_height,
                                    &image_channel, image_channel);
    if (!data) {
        std::cerr << "ERROR: Could not load texture image file '" << std::endl;
//        stbi_image_free(data);
        image_width = image_height = 0;
    }

    int bytes_per_scanline = image_channel * image_width;
    std::cout << image_width << " " << image_height << " " << image_channel << std::endl;

    for (int j = image_height - 1; j >= image_height / 2; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto pixel_data = data + j * bytes_per_scanline + i * image_channel;
            data[j * bytes_per_scanline + i * image_channel + 1] = 0;
        }
    }

    stbi_write_png(FileSystem::getPath("test.png").c_str(), image_width, image_height, image_channel, data, 0);
    stbi_image_free(data);
};

static glm::vec4 veclerp(const glm::vec4 &v1, const glm::vec4 &v2, float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}

static glm::vec3 veclerp(const glm::vec3 &v1, const glm::vec3 &v2, float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}

static glm::vec2 veclerp(const glm::vec2 &v1, const glm::vec2 &v2, float factor) {
    return (1.0f - factor) * v1 + factor * v2;
}

static float
interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3, const float num1, const float num2,
            const float num3, float weight) {
    return (alpha * num1 / z1 + beta * num2 / z2 + gamma * num3 / z3) * weight;
}

static glm::vec2 interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3, const glm::vec2 &vert1,
                             const glm::vec2 &vert2,
                             const glm::vec2 &vert3, float weight) {
    return (alpha * vert1 / z1 + beta * vert2 / z2 + gamma * vert3 / z3) * weight;
}

static glm::vec3 interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3, const glm::vec3 &vert1,
                             const glm::vec3 &vert2,
                             const glm::vec3 &vert3, float weight) {
    return (alpha * vert1 / z1 + beta * vert2 / z2 + gamma * vert3 / z3) * weight;
}

static glm::vec4 interpolate(float alpha, float beta, float gamma, float z1, float z2, float z3, const glm::vec4 &vert1,
                             const glm::vec4 &vert2,
                             const glm::vec4 &vert3, float weight) {
    return (alpha * vert1 / z1 + beta * vert2 / z2 + gamma * vert3 / z3) * weight;
}


#endif //RAYTRACING_UTILS_H
