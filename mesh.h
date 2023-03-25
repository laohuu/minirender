#ifndef RAYTRACING_MESH_H
#define RAYTRACING_MESH_H

#include "vertex.h"
#include "texture.h"
#include "rasterizer.h"

class mesh {
public:
    // mesh Data
    vector<vertex> vertices;
    vector<unsigned int> indices;
    vector<shared_ptr<texture>> textures;

    // constructor
    mesh(vector<vertex> vertices, vector<unsigned int> indices, vector<shared_ptr<texture>> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

//        setupmesh();
    }

    // render the mesh
    void draw(rasterizer &raster) {
        for (int i = 0; i < indices.size(); i += 3) {
            vertex p1, p2, p3;
            p1 = vertices[indices[i]];
            p2 = vertices[indices[i + 1]];
            p3 = vertices[indices[i + 2]];

            shared_ptr<texture> _tex;
            for (const auto &texture: textures) {
                string number;
                string name = texture->type;
                if (name == "texture_diffuse") {
                    _tex = texture;
                }
            }

//            raster.render_line(p1,p2);
//            raster.render_line(p2,p3);
//            raster.render_line(p3,p1);

            raster.set_texture(_tex);
            raster.render_triangle(p1, p2, p3);

        }
    }

private:

};

#endif //RAYTRACING_MESH_H
