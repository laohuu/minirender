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

    virtual glm::vec4 get_value(double u, double v) const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}

    solid_color(glm::vec4 color) : color_value(color) {}

    solid_color(float red, float green, float blue)
            : solid_color(glm::vec4(red, green, blue, 1.0f)) {}

    glm::vec4 get_value(double u, double v) const override {
        return color_value;
    }

private:
    glm::vec4 color_value;
};

class image_texture : public texture {
public:
    image_texture(const string &path) {
        this->path = path;
        color_data = stbi_load(
                path.c_str(), &width, &height, &channel, 0);

        std::cout << "load texture '" << path << "' .\n";
        if (!color_data) {
            std::cerr << "ERROR: Could not load texture image file '" << path << "'.\n";
            width = height = 0;
        }
    }

    image_texture() : color_data(nullptr), width(0), height(0), channel(0) {}

    ~image_texture() {
        if (color_data)
            stbi_image_free(color_data);
        color_data = nullptr;
    }

private:
    int width, height, channel;
    unsigned char *color_data;

    glm::vec4 set_color(unsigned char *data) {
        if (color_data)
            delete[] color_data;
        color_data = new unsigned char[width * height * channel];
        memcpy(color_data, data, width * height * channel);
    }

    glm::vec4 get_value(double u, double v) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (color_data == nullptr)
            return glm::vec4(0.f, 1.0f, 1.0f, 1.0f);;

         //reapte
//        float u = u - std::floor(u);
//        float v =v - std::floor(v);

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
        return color;
    }

};

#endif //RAYTRACING_TEXTURE_H
