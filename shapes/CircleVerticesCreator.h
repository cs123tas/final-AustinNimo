#ifndef CIRCLEVERTICESCREATOR_H
#define CIRCLEVERTICESCREATOR_H

#include "lib/Vertex.h"
#include <vector>

class CircleVerticesCreator
{
public:
    CircleVerticesCreator();
    ~CircleVerticesCreator();
protected:
    std::vector<Vertex> buildSphereVertices(int numHeight, int numWidth, float radius);

    std::vector<Vertex> buildTorusVertices(int numWidth, int numHeight, float radius, float radiusPercentage);
};

#endif // CIRCLEVERTICESCREATOR_H
