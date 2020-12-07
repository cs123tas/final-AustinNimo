#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <unordered_map>
#include "lib/Vertex.h"
#include <vector>
#include <string>
#include "glm.hpp"

class MeshLoader
{
public:
    MeshLoader();
    void loadMesh(char* fileName);
    std::vector<Vertex> getVertices();
    std::vector<int> getIndices();
private:
    std::unordered_map<Vertex,int, Vertex> m_vertexMap;
    std::vector<Vertex> m_vertices;
    std::vector<int> m_indices;
    std::vector<glm::vec3> m_tempXYZ;
    std::vector<glm::vec2> m_tempUV;
    std::vector<glm::vec3> m_tempXYZN;
    void addVertex(std::string line);
    void addVertexUV(std::string line);
    void addVertexNormal(std::string line);
    void addFace(std::string line);
    std::vector<std::string> splitString(std::string line, char character);
    int countChars(std::string line, char character);

};

#endif // MESHLOADER_H
