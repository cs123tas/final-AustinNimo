#ifndef SHAPE_H
#define SHAPE_H

/** imports the OpenGL math library https://glm.g-truc.net/0.9.2/api/a00001.html */
#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>
#include "lib/Vertex.h"
#include "gl/datatype/VBO.h"
#include "gl/datatype/VAO.h"
#include "gl/datatype/IBO.h"
#include "gl/datatype/VBOAttribMarker.h"
#include "gl/shaders/ShaderAttribLocations.h"

/**
 *
 * inserts a glm::vec3 into a vector of floats
 * this will come in handy if you want to take advantage of vectors to build your shape
 * make sure to call reserve beforehand to speed this up
 */
void insertVec3(std::vector<float> &data, glm::vec3 v);

namespace CS123 { namespace GL {
class VAO;
}}

class Shape
{
public:
    Shape();
    ~Shape();
    void draw();
    std::vector<Vertex> getVertexData();
    std::vector<Vertex> m_vertexData;
    /** builds the VAO, pretty much the same as from lab 1 */
    void buildVAO(CS123::GL::VBO::GEOMETRY_LAYOUT layout=CS123::GL::VBO::LAYOUT_TRIANGLE_STRIP);

protected:

    std::vector<int> m_vertexLocs;
    std::unique_ptr<CS123::GL::VAO> m_VAO;
};

#endif // SHAPE_H
