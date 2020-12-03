#include "Sphere.h"

Sphere::Sphere()
{

}
Sphere::~Sphere()
{

}
Sphere::Sphere(int param1, int param2)
{
    if (param1 < 2) {
        param1 = 2;
    }
    if (param2 < 3) {
        param2 = 3;
    }

    m_vertexData = buildSphereVertices(param1, param2, .5);
    m_vertexLocs.clear();

    // build the top
    int top = m_vertexData.size() - (param2 + 1) * 2;
    for (int i = 0; i <= param2; i++) {
        m_vertexLocs.push_back(top + i);
        m_vertexLocs.push_back(i + 1);
        m_vertexLocs.push_back(i);
        m_vertexLocs.push_back(0xFFFFFF);
    }

    // build the bottom
    int bottom = m_vertexData.size() - (param2 + 1);
    for (int i = 0; i <= param2; i++) {
        m_vertexLocs.push_back(bottom + i);
        m_vertexLocs.push_back((param2 + 1) * (param1 - 2) + i);
        m_vertexLocs.push_back((param2 + 1) * (param1 - 2) + i + 1);
        m_vertexLocs.push_back(0xFFFFFF);
    }

    // build the middle
    for (int i = 0; i < param1 - 2; i++) {
        for (int j = 0; j <= param2; j++) {
            m_vertexLocs.push_back((i + 1) * (param2 + 1) + j);
            m_vertexLocs.push_back( i* (param2 + 1) + j);
        }
//        m_vertexLocs.push_back(i * param2 + param2);
//        m_vertexLocs.push_back(i * param2);
        m_vertexLocs.push_back(0xFFFFFF);
    }

    /** build the VAO so that the shape is ready to be drawn */
    buildVAO();

}
