#include "utils.h"
#include "enable_stb_image.h"
#include "filesystem.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "framebuffer.h"
#include <iostream>
#include "model.h"
#include "rasterizer.h"
#include "camera.h"


glm::mat4 get_model_matrix(glm::vec3 world_pos, float angle_x, float angle_y, float angle_z) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, world_pos);
    model = glm::rotate(model, glm::radians(angle_z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
    return model;
}

int main() {
    // load models
    // ----------
    model our_model(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
//    model our_model(FileSystem::getPath("resources/objects/cube/cube.obj"));

    // Image
    const float aspect_ratio = 16.0 / 9.0;
    const int image_width = 1280;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int image_channel = 3;
    float eye_fov = 45.f;
    camera cam(glm::vec3(0, 0, 5.f), glm::vec3(0.f), eye_fov, aspect_ratio, 0.01f, 100.0f);

    //Light
    shared_ptr<direction_light> _light = make_shared<direction_light>();
    shared_ptr<point_light> point_li = make_shared<point_light>();
    shared_ptr<point_light> point_li2 = make_shared<point_light>(glm::vec3(0, -2.f, 2.0f));
    shared_ptr<spot_light> spot_li = make_shared<spot_light>();
    shared_ptr<shader> render = make_shared<blinn_phong_shader>();
    render->push_dir_light(_light);
    render->push_point_light(point_li);
    render->push_point_light(point_li2);
    render->push_spot_light(spot_li);

//    raster.
    rasterizer raster(image_width, image_height, image_channel, render);
    glm::vec4 background_color(0.05f, 0.05f, 0.05f, 1.0f);
    raster.clear_color_buffer(background_color);

    glm::mat4 model_transformation = get_model_matrix(glm::vec3(0.0f, 0.0f, 0.0f), -0.0f, 0.0f, 0.0f);
    glm::mat4 view = cam.get_view_matrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);

    raster.set_model_matrix(model_transformation);
    raster.set_view_matrix(view);
    raster.set_projection_matrix(projection);

    our_model.draw(raster);

    raster.ouput_image();

    std::cerr << "\nDone.\n";
    return 0;
}


