#ifndef VERTEX_H
#define VERTEX_H
#include <limits>
#include <tuple>
#include <glm.hpp>
#include "GL/glew.h"

struct Vertex
{
    Vertex() {
        XYZ = {0.0,0.0,0.0};
        UV = {0.0,0.0};
        XYZN = {0.0,0.0,0.0};
    }
    Vertex(glm::vec3 XYZ, glm::vec2 UV, glm::vec3 XYZN) {
        this->XYZ = XYZ;
        this->UV = UV;
        this->XYZN = XYZN;
    }
    bool operator!=(const Vertex& p_rhs) const
        {
            return !(XYZ.x == p_rhs.XYZ.x && XYZ.y == p_rhs.XYZ.y && XYZ.z == p_rhs.XYZ.z && UV.x == p_rhs.UV.x && UV.y == p_rhs.UV.y && XYZN.x == p_rhs.XYZN.x && XYZN.y == p_rhs.XYZN.y && XYZN.z == p_rhs.XYZN.z);
        }
    bool operator==(const Vertex& p_rhs) const
        {
            return (XYZ.x == p_rhs.XYZ.x && XYZ.y == p_rhs.XYZ.y && XYZ.z == p_rhs.XYZ.z && UV.x == p_rhs.UV.x && UV.y == p_rhs.UV.y && XYZN.x == p_rhs.XYZN.x && XYZN.y == p_rhs.XYZN.y && XYZN.z == p_rhs.XYZN.z);
        }

    size_t operator()(const Vertex& x) const {
            std::size_t seed = 0;
            seed = XYZ.x * 5 + XYZ.y * 7 + XYZ.z * 11 + UV.x * 13 + UV.y * 17 + XYZN.x * 19 + XYZN.y * 23 + XYZN.z * 27;
            return seed;
        }

  glm::vec3 XYZ;
  glm::vec2 UV;
  glm::vec3 XYZN;
} ;

const GLfloat MAX_FLOAT = std::numeric_limits<GLfloat>::max();
const Vertex SKIP_VERTEX = {{MAX_FLOAT, MAX_FLOAT, MAX_FLOAT},{MAX_FLOAT, MAX_FLOAT}, {MAX_FLOAT,MAX_FLOAT,MAX_FLOAT}};
const int SKIP_INDEX = 0xFFFFFF;

enum plane {X, Y, Z};

#endif // VERTEX_H
