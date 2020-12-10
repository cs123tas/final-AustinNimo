#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr
#include <vector>

#include "openglshape.h"

class Terrain {
public:
    Terrain();

    void init();
    void draw();

private:
    float randValue(int row, int col);
    float getNoiseValue(int row, int col, float octave);
    glm::vec3 getPosition(int row, int col);
    glm::vec3 getNormal(int row, int col);
    std::unique_ptr<OpenGLShape> m_shape;
    const float m_numRows, m_numCols;
};

#endif // TERRAIN_H
