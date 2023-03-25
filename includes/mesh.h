#ifndef RAYTRACING_MESH_H
#define RAYTRACING_MESH_H

#include "vertex.h"
#include "material.h"
#include "rasterizer.h"

class mesh {
public:
    // mesh Data
    vector<vertex> vertices;
    vector<unsigned int> indices;
    shared_ptr<material> _material;

    // constructor
    mesh(vector<vertex> vertices, vector<unsigned int> indices, shared_ptr<material> _mat) : _material(
            _mat) {
        this->vertices = vertices;
        this->indices = indices;
    }

    // render the mesh
    void draw(rasterizer &raster) {
        for (int i = 0; i < indices.size(); i += 3) {
            vertex p1, p2, p3;
            p1 = vertices[indices[i]];
            p2 = vertices[indices[i + 1]];
            p3 = vertices[indices[i + 2]];
            raster.set_material(_material);
            raster.render_triangle(p1, p2, p3);
//            raster.wireframe_triangle(p1, p2, p3);
        }
    }

private:

};

#endif //RAYTRACING_MESH_H
