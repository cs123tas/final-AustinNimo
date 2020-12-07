#include "MeshLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::unordered_map<Vertex, int, Vertex> m_vertexMap;
std::vector<Vertex> m_vertices;
std::vector<int> m_indices;
std::vector<glm::vec3> m_tempXYZ;
std::vector<glm::vec2> m_tempUV;
std::vector<glm::vec3> m_tempXYZN;

MeshLoader::MeshLoader()
{
}

// Open a file and load an obj
void MeshLoader::loadMesh(char* fileName) {
    std::ifstream objFile;
    objFile.open(fileName);

    if (objFile.is_open())
    {
        std::string line;
        while ( getline (objFile,line) )
            {
              auto token = line[0];
              if (token == 'v') {
                  token = line[1];
                    if (token == ' ') {
                        addVertex(line);
                    } else if (token == 't') {
                        addVertexUV(line);
                    } else if (token == 'n') {
                        addVertexNormal(line);
                    }
              } else if (token == 'f') {
                  addFace(line);
              }
            }
    }
    else std::cout << "Unable to open file";
}

// Vertices are in the form x y z
void MeshLoader::addVertex(std::string line) {

    std::vector<std::string> words = splitString(line, ' ');
    glm::vec3 temp = {std::stof(words[1]), std::stof(words[2]), std::stof(words[3])};
    m_tempXYZ.push_back(temp);
}

void MeshLoader::addVertexUV(std::string line) {

    std::vector<std::string> words = splitString(line, ' ');
    glm::vec2 temp = {std::stof(words[1]), std::stof(words[2])};
    m_tempUV.push_back(temp);
}

void MeshLoader::addVertexNormal(std::string line) {

    std::vector<std::string> words = splitString(line, ' ');
    glm::vec3 temp = {std::stof(words[1]), std::stof(words[2]), std::stof(words[3])};
    m_tempXYZN.push_back(temp);
}

// Faces are eiter in the form XYZ index, X/U index, or X/U/Normals index. They use a fan shape from a center point.
void MeshLoader::addFace(std::string line) {
    std::vector<std::string> words = splitString(line, ' ');
    for (auto word : words) {
        if (word[0] != 'f') {
            int slashes = countChars(word, '/');
            std::vector<std::string>vertexInfo = splitString(word, '/');
            Vertex temp;
            if (slashes == 0) {
                temp.XYZ = m_tempXYZ[std::stoi(vertexInfo[0]) - 1];
                temp.UV = {0,0};
                temp.XYZN = {0,0,0};
            } else if (slashes == 1) {
                temp.XYZ = m_tempXYZ[std::stoi(vertexInfo[0]) - 1];
                temp.UV = m_tempUV[std::stoi(vertexInfo[1]) - 1];
                temp.XYZN = {0,0,0};
            } else if (slashes == 2) {
                temp.XYZ = m_tempXYZ[std::stoi(vertexInfo[0]) - 1];
                if (vertexInfo[1] != "" ){
                    temp.UV = m_tempUV[std::stoi(vertexInfo[1]) - 1];
                } else {
                    temp.UV = {0,0};
                }
                temp.XYZN = m_tempXYZN[std::stoi(vertexInfo[2]) - 1];
            }
            std::unordered_map<Vertex,int, Vertex>::const_iterator got = m_vertexMap.find (temp);
            if ( got == m_vertexMap.end() ) {
                m_vertexMap.insert({temp, m_vertices.size()});
                m_indices.push_back(m_vertices.size());
                m_vertices.push_back(temp);
            }
            else {
                m_indices.push_back(got->second);
            }
        }
    }
    m_indices.push_back(SKIP_INDEX);
}

std::vector<std::string> MeshLoader::splitString(std::string line, char character) {
    std::vector<std::string> words;
    std::istringstream stream(line);
    std::string word;
    while (getline(stream, word, character)) {
            words.push_back(word);
    }
    return words;
}

int MeshLoader::countChars(std::string line, char character) {
    int currentPosition = line.find(character, 0);
    int count = 0;

    while (currentPosition != (int)std::string::npos)
    {
        count++;
        currentPosition = line.find(character, currentPosition + 1);
    }
    return count;
}

std::vector<Vertex> MeshLoader::getVertices() {
    return m_vertices;
}

std::vector<int> MeshLoader::getIndices() {
    return m_indices;
}
