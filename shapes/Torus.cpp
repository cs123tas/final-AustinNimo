#include "Torus.h"

Torus::Torus()
{

}
Torus::~Torus()
{

}
Torus::Torus(int param1, int param2, float param3)
{
    if (param1 < 3) {
        param1 = 3;
    }
    if (param2 < 3) {
        param2 = 3;
    }

    m_vertexData = buildTorusVertices(param1, param2, .5, param3);
    m_vertexLocs.clear();

    // build the middle
    for (int i = 0; i < param2; i++) {
        for (int j = 0; j < param1; j+= 1) {
            m_vertexLocs.push_back(j * param2 + ((i + 1) % param2));
            m_vertexLocs.push_back(j * param2 + i);
        }
        m_vertexLocs.push_back((i + 1) % param2);
        m_vertexLocs.push_back(i);
        m_vertexLocs.push_back(0xFFFFFF);
    }

    /** build the VAO so that the shape is ready to be drawn */
    buildVAO();
}
