#include "Cone.h"
#include <cmath>

// Times .5 to get into the scale of the triangle
const float NORMAL_Y = 1 /sqrt(5) * .5;
const glm::vec3 BASE_NORMAL = {0, -1, 0};

Cone::Cone()
{
}

Cone::Cone(int param1, int param2)
{
    if (param2 < 3) {
        param2 = 3;
    }
    glm::vec3 tip = {0, .5, 0};
    glm::vec3 base = {0, -.5, 0};
    glm::vec3 leftVertex;
    glm::vec3 rightVertex;
    glm::vec3 leftNormal;
    glm::vec3 rightNormal;
    m_vertexData.clear();

    float degreeStride = 2 * M_PI / param2;
    float degrees = 0;
    int vertices = 0;
    float widthPercentage = 1.0 / param2;

    for(int i = 0; i < param2; i++) {

        // Make the body of the cone
        leftVertex = {.5 * cos(degrees), -.5, .5 * sin(degrees)};
        degrees += degreeStride;
        rightVertex = {.5 * cos(degrees), -.5, .5 * sin(degrees)};
        // Multiplication by .5 to get into the scale of the triangle
        leftNormal = {leftVertex.x, NORMAL_Y, leftVertex.z};
        rightNormal = {rightVertex.x, NORMAL_Y , rightVertex.z};
        leftNormal = glm::normalize(leftNormal);
        rightNormal = glm::normalize(rightNormal);
        std::vector<Vertex> vertexData = buildTriangleStrip(tip, leftVertex, rightVertex, leftNormal,rightNormal,param1);
        m_vertexData.insert(m_vertexData.end(), vertexData.begin(), vertexData.end());
        // Deal with tip seperately
        m_vertexData[vertices].UV.x =  1 - (widthPercentage * i);
        m_vertexData[vertices].UV.y =  1;
        vertices++;
        for(; vertices < (int)m_vertexData.size();vertices++) {
            if (m_vertexData[vertices] != SKIP_VERTEX) {
                float theta = atan2(m_vertexData[vertices].XYZ.z, m_vertexData[vertices].XYZ.x);
                if (theta < 0) {
                    m_vertexData[vertices].UV.x = -theta / (2.0*M_PI);
                } else {
                    m_vertexData[vertices].UV.x = 1.0 - (theta / (2.0*M_PI));
                }
    //            m_vertexData[vertices].UV.x = atan2(m_vertexData[vertices].XYZ.z, m_vertexData[vertices].XYZ.x) / (2.0*M_PI);
                m_vertexData[vertices].UV.y = m_vertexData[vertices].XYZ.y + .5;
            }
        }

        // Make the base of the cone, vertexes reversed since the normals have changed
        vertexData = buildTriangleStrip(base, rightVertex, leftVertex, BASE_NORMAL, BASE_NORMAL, param1);
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

Cone::~Cone()
{
}
