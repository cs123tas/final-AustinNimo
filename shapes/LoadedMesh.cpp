#include "LoadedMesh.h"
#include "gl/datatype/VBO.h"

LoadedMesh::LoadedMesh()
{

}


LoadedMesh::~LoadedMesh(){

}
LoadedMesh::LoadedMesh(std::vector<Vertex> vertices, std::vector<int> indices) {
    m_vertexData = vertices;
    m_vertexLocs = indices;
    buildVAO(CS123::GL::VBO::LAYOUT_TRIANGLE_FAN);
}
