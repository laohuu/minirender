#ifndef RAYTRACING_VERTEX_H
#define RAYTRACING_VERTEX_H

#include "glm/glm.hpp"

#define MAX_BONE_INFLUENCE 4

class vertex {
public:
    glm::vec4 position;
    glm::vec4 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

    vertex() = default;

    ~vertex() = default;

    vertex(
            const glm::vec4 &_pos,
            const glm::vec4 &_color,
            const glm::vec2 &_tex,
            const glm::vec3 &_normal
    ) :
            position(_pos), color(_color), texcoord(_tex), normal(_normal) {}

    vertex(
            const glm::vec3 &_pos,
            const glm::vec4 &_color = glm::vec4(0, 0, 0, 0),
            const glm::vec2 &_tex = glm::vec2(0, 0),
            const glm::vec3 &_normal = glm::vec3(0, 0, 1)
    ) :
            position(_pos, 1.0f), color(_color), texcoord(_tex), normal(_normal) {}

    vertex(const vertex &v) : position(v.position), color(v.color), texcoord(v.texcoord), normal(v.normal) {}
};

#endif //RAYTRACING_VERTEX_H
