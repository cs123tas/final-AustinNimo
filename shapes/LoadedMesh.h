#ifndef LOADEDMESH_H
#define LOADEDMESH_H

#include "Shape.h"
#include "lib/Vertex.h"
#include <vector>

class LoadedMesh : public Shape
{
public:
    LoadedMesh();
    ~LoadedMesh();
    LoadedMesh(std::vector<Vertex> vertices, std::vector<int> indices);
};

#endif // LOADEDMESH_H
