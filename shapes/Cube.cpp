#include "Cube.h"

Cube::Cube()
{
}

Cube::Cube(int param1) :
    m_param1(param1)
{
    /**
     * We build in vertex data for a cube, in this case they are handwritten.
     * You'll want to expand upon or refactor this code heavily to take advantage
     * of polymorphism, vector math/glm library, and utilize good software design
     *
     */
//    m_vertexData = CUBE_DATA_POSITIONS;
//    m_vertexLocs = Indices;

    Vertex start = {{.5, -.5, -.5},{0,0},{0,0,-1}};
    Vertex end = {{-.5, .5, -.5},{0,0},{0,0,-1}};
    std::vector<Vertex> vertexData = buildRectanglePlane(start, end, param1, param1, 1, 0, 2);
    int vertices = 0;
    m_vertexData = vertexData;
    for(; vertices < (int)m_vertexData.size();vertices++) {
        if (m_vertexData[vertices] != SKIP_VERTEX) {
            m_vertexData[vertices].UV.x = 1 - (m_vertexData[vertices].XYZ.x + .5);
            m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.y + .5;
        }
    }
    start = {{-.5, -.5, .5},{0,0},{0,0,1}};
    end = {{.5, .5, .5},{0,0},{0,0,1}};
    vertexData = buildRectanglePlane(start, end, param1, param1, 1, 0, 2);
    m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
    for(; vertices < (int)m_vertexData.size();vertices++) {
        if (m_vertexData[vertices] != SKIP_VERTEX) {
            m_vertexData[vertices].UV.x = m_vertexData[vertices].XYZ.x + .5;
            m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.y + .5;
        }
    }
    start = {{-.5, -.5, -.5},{0,0},{-1,0,0}};
    end = {{-.5, .5, .5},{0,0},{-1,0,0}};
    vertexData = buildRectanglePlane(start, end, param1, param1, 1, 2, 0);
    m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
    for(; vertices < (int)m_vertexData.size();vertices++) {
        if (m_vertexData[vertices] != SKIP_VERTEX) {
            m_vertexData[vertices].UV.x = (m_vertexData[vertices].XYZ.z + .5);
            m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.y + .5;
        }
    }
    start = {{.5, -.5, .5},{0,0},{1,0,0}};
    end = {{.5, .5, -.5},{0,0},{1,0,0}};
    vertexData = buildRectanglePlane(start, end, param1, param1, 1, 2, 0);
    m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
    for(; vertices < (int)m_vertexData.size();vertices++) {
        if (m_vertexData[vertices] != SKIP_VERTEX) {
            m_vertexData[vertices].UV.x = 1 - (m_vertexData[vertices].XYZ.z + .5);
            m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.y + .5;
        }
    }
    start = {{-.5, -.5, .5},{0,0},{0,-1,0}};
    end = {{.5, -.5, -.5},{0,0},{0,-1,0}};
    vertexData = buildRectanglePlane(start, end, param1, param1, 0, 2, 1);
    m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
    for(; vertices < (int)m_vertexData.size();vertices++) {
        if (m_vertexData[vertices] != SKIP_VERTEX) {
            m_vertexData[vertices].UV.x = (m_vertexData[vertices].XYZ.x + .5);
            m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.z + .5;
        }
    }
    start = {{-.5, .5, -.5},{0,0},{0,1,0}};
    end = {{.5, .5, .5},{0,0},{0,1,0}};
    vertexData = buildRectanglePlane(start, end, param1, param1, 0, 2, 1);
    m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
    for(; vertices < (int)m_vertexData.size();vertices++) {
        if (m_vertexData[vertices] != SKIP_VERTEX) {
            m_vertexData[vertices].UV.x = (m_vertexData[vertices].XYZ.x + .5);
            m_vertexData[vertices].UV.y = 1 - (m_vertexData[vertices].XYZ.z + .5);
        }
    }
    m_vertexLocs.clear();
    // Want to stop before the last one so that the last skip is ignored.
    for (int i = 0; i < (int)m_vertexData.size(); i++) {
        if (m_vertexData[i] != SKIP_VERTEX) {
            m_vertexLocs.push_back(i);
        } else {
            m_vertexLocs.push_back(0xFFFFFF);
        }
    }

    /** build the VAO so that the shape is ready to be drawn */
    buildVAO();
}

Cube::~Cube()
{
}
