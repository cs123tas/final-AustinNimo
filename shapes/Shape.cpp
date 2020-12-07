#include "Shape.h"

using namespace CS123::GL;

void insertVec3(std::vector<float> &data, glm::vec3 v){
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

Shape::Shape() :
    m_VAO(nullptr)
{

}

Shape::~Shape()
{
}

void Shape::draw() {
    if (m_VAO) {
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}

void Shape::buildVAO(VBO::GEOMETRY_LAYOUT layout) {
    const int numFloatsPerVertex = 8;
    const int numVertices = m_vertexLocs.size();
    const size_t positionSize = sizeof(m_vertexData[0].XYZ);
    const size_t texSize = sizeof(m_vertexData[0].UV);
    const size_t texOffset = positionSize;
    const size_t normalOffset = positionSize + texSize;
    std::vector<VBOAttribMarker> markers;
    markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    markers.push_back(VBOAttribMarker(ShaderAttrib::TEXCOORD0, 3, texOffset));
    markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, normalOffset));
    VBO vbo = VBO(m_vertexData.data(), m_vertexData.size() * numFloatsPerVertex, markers, layout);
    IBO ibo = IBO(m_vertexLocs.data(), (int)m_vertexLocs.size());
    m_VAO = std::make_unique<VAO>(vbo, ibo, numVertices);
}


std::vector<Vertex> Shape::getVertexData() {
    return m_vertexData;
}
