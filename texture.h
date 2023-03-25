#ifndef RAYTRACING_TEXTURE_H
#define RAYTRACING_TEXTURE_H

#include "glm/glm.hpp"
#include "string"
#include "utils.h"

using namespace std;

class texture {
public:
    unsigned int id;
    string type;
    string path;
    int width, height, channel;
    unsigned char *color_data;

    texture() : color_data(nullptr) {};

    ~texture() {
        if (color_data)
            delete[] color_data;
    };

    texture(const string &path) : path(path) {

    }

    texture(
            const unsigned int &id,
            const string &type,
            const string &path
    ) :
            id(id), type(type), path(path) {}

    texture(const texture &tex) : id(tex.id), type(tex.type), path(tex.path) {}

    glm::vec4 set_color(unsigned char *data) {
        if (color_data)
            delete[] color_data;
        color_data = new unsigned char[width * height * channel];
        memcpy(color_data, data, width * height * channel);
    }

    glm::vec4 get_color(float u, float v) {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (color_data == nullptr)
            return glm::vec4(0.f, 1.0f, 1.0f, 1.0f);;

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0f, 1.0f);
        v = 1.0f - clamp(v, 0.0f, 1.0f);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width) i = width - 1;
        if (j >= height) j = height - 1;

        const float color_scale = 1.0 / 255.0;
        auto pixel_data = color_data + j * width * channel + i * channel;
        glm::vec4 color(1.0f);
        for (int k = 0; k < channel; ++k) {
            color[k] = *pixel_data++ * color_scale;
        }
//        float w = color.w;
//        color = color * color_scale;
//        color.w = w;
        return color;
    }

};

#endif //RAYTRACING_TEXTURE_H
