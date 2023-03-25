#ifndef RAYTRACING_FRAMEBUFFER_H
#define RAYTRACING_FRAMEBUFFER_H

#include "iostream"
#include "vector"
#include "glm/glm.hpp"
#include "utils.h"

class framebuffer {
public:
    int width, height, channel;
    unsigned char *buffer_data;
    std::vector<float> depth_buffer;


    ~framebuffer() {
        delete[] buffer_data;
    };

    framebuffer(const int &w = 800, const int &h = 600, const int &c = 3) : width(w), height(h), channel(c) {
        //RGB
        buffer_data = new unsigned char[width * height * channel];
        depth_buffer.resize(w * h, 1.0f);
    }

    void resize_buffer(const int &w, const int &h) {
        width = w;
        height = h;

        delete[] buffer_data;
        buffer_data = new unsigned char[width * height * channel];
        depth_buffer.resize(w * h, 1.0f);
    }

    float get_depth(const int &x, const int &y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return 1.0;
        return *(depth_buffer.data() + y * width + x);
    }

    void write_depth(const int &x, const int &y, const float &depth) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return;
        float *p = depth_buffer.data();
        *(p + y * width + x) = depth;
    }

    void clear_color_buffer(const glm::vec4 &color) {
        int length = width * height * channel;
        auto pixel_data = buffer_data;
        for (int i = 0; i < length; i += channel) {
            for (int c = 0; c < channel; ++c) {
                auto channel_value = std::pow(color[c], 1.0f / 2.2f);
                *pixel_data++ = static_cast<unsigned char>(std::clamp(channel_value * 255.99, 0.0, 255.0));
            }
        }
    }

    void set_pixel(int x, int y, glm::vec4 pixel_color, int samples_per_pixel = 1) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return;
        auto pixel_data = buffer_data + y * width * channel + x * channel;
        auto scale = 1.0f / samples_per_pixel;

        for (int i = 0; i < channel; ++i) {
//            auto value = std::pow(scale * pixel_color[i], 1.0f / 2.2f);
            auto value = pixel_color[i];
            *pixel_data++ = static_cast<unsigned char>(std::clamp(value * 255.0, 0.0, 255.0));
        }
    }

};

#endif //RAYTRACING_FRAMEBUFFER_H
