#ifndef RAYTRACING_VERTEX2FRAGMENT_H
#define RAYTRACING_VERTEX2FRAGMENT_H

#include "glm/glm.hpp"
#include "utils.h"

class vertex2fragment {
public:
    glm::vec4 world_pos;
    glm::vec4 view_pos;
    glm::vec4 projection_pos;
    glm::vec4 viewport_pos;
    glm::vec4 color;
    glm::vec2 texcoord;
    glm::vec3 normal;

    vertex2fragment() = default;

    ~vertex2fragment() = default;

    vertex2fragment(
            const glm::vec4 &_wPos,
            const glm::vec4 &_pPos,
            const glm::vec4 &_color,
            const glm::vec2 &_tex,
            const glm::vec3 &_normal
    ) :
            world_pos(_wPos), projection_pos(_pPos), color(_color), texcoord(_tex), normal(_normal) {}

    vertex2fragment(const vertex2fragment &v) :
            world_pos(v.world_pos), projection_pos(v.projection_pos), color(v.color), texcoord(v.texcoord),
            normal(v.normal) {}

    static vertex2fragment lerp(const vertex2fragment &v1, const vertex2fragment &v2, const float &factor) {
        vertex2fragment result;
        result.projection_pos = veclerp(v1.projection_pos, v2.projection_pos, factor);
        result.world_pos = veclerp(v1.world_pos, v2.world_pos, factor);
        result.color = veclerp(v1.color, v2.color, factor);
        result.normal = veclerp(v1.normal, v2.normal, factor);
        result.texcoord = veclerp(v1.texcoord, v2.texcoord, factor);
        result.viewport_pos = veclerp(v1.viewport_pos, v2.viewport_pos, factor);
        return result;
    }
};


#endif //RAYTRACING_VERTEX2FRAGMENT_H
