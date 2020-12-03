#include "Cylinder.h"
#include "iostream"

const glm::vec3 TOP_NORMAL = {0, 1, 0};
const glm::vec3 BASE_NORMAL = {0, -1, 0};
const glm::vec3 TOP = {0, .5, 0};
const glm::vec3 BASE = {0, -.5, 0};

Cylinder::Cylinder()
{

}

Cylinder::Cylinder(int param1, int param2)
{
    if (param2 < 3) {
        param2 = 3;
    }
    Vertex leftVertex;
    Vertex rightVertex;
    glm::vec3 leftNormal;
    glm::vec3 rightNormal;
    m_vertexData.clear();

    float degreeStride = 2 * M_PI / param2;
    float degrees = 0;

    int vertices = 0;
    for(int i = 0; i < param2; i++) {

        // Make the body of the cylinder
        leftVertex = {{.5 * cos(degrees), .5, .5 * sin(degrees)}, {0.0,0.0}, {.5 * cos(degrees),0,.5 * sin(degrees)}};
        degrees += degreeStride;
        rightVertex = {{.5 * cos(degrees), -.5, .5 * sin(degrees)}, {0.0,0.0}, {.5 * cos(degrees),0,.5 * sin(degrees)}};

        leftVertex.XYZN = glm::normalize(leftVertex.XYZN);
        rightVertex.XYZN = glm::normalize(rightVertex.XYZN);

        std::vector<Vertex> vertexData = buildRectanglePlane(leftVertex, rightVertex, param1,1,1,2,0);
        m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
        for(; vertices < (int)m_vertexData.size();vertices++) {

            if (m_vertexData[vertices] != SKIP_VERTEX) {
                float theta = atan2(m_vertexData[vertices].XYZ.z, m_vertexData[vertices].XYZ.x);

                if (theta < 0) {
                    m_vertexData[vertices].UV.x = -theta / (2.0*M_PI);
                } else {
                    m_vertexData[vertices].UV.x = 1.0 - (theta / (2.0*M_PI));
                }
                m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.y + .5;
                if ((i > 1 && (m_vertexData[vertices].UV.x >= .99)) ||  m_vertexData[vertices].UV.x <= 0.0) {
                    m_vertexData[vertices].UV.x = 0;
                }
            }
        }

        // Make the top of the cylinder
        leftVertex.XYZ.y = .5;
        rightVertex.XYZ.y = .5;
        vertexData = buildTriangleStrip(TOP, leftVertex.XYZ, rightVertex.XYZ, TOP_NORMAL, TOP_NORMAL, param1);
        m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());

        for(; vertices < (int)m_vertexData.size();vertices++) {
            if (m_vertexData[vertices] != SKIP_VERTEX)  {
                m_vertexData[vertices].UV.x = m_vertexData[vertices].XYZ.x + .5;
                m_vertexData[vertices].UV.y = 1 - (m_vertexData[vertices].XYZ.z + .5);
            }
        }

        // Make the base of the cylinder, vertexes reversed since the normals have changed
        leftVertex.XYZ.y = -.5;
        rightVertex.XYZ.y = -.5;
        vertexData = buildTriangleStrip(BASE, rightVertex.XYZ, leftVertex.XYZ, BASE_NORMAL, BASE_NORMAL, param1);
        m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());

        for(; vertices < (int)m_vertexData.size();vertices++) {
            if (m_vertexData[vertices] != SKIP_VERTEX) {
                m_vertexData[vertices].UV.x = m_vertexData[vertices].XYZ.x + .5;
                m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.z + .5;
            }
        }

    }

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

Cylinder::~Cylinder()
{
}
