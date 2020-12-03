#ifndef TRIANGLECREATOR_H
#define TRIANGLECREATOR_H

#include "lib/Vertex.h"
#include <vector>

class TriangleCreator
{
public:
    TriangleCreator();
protected:
    std::vector<Vertex> buildTriangleStrip(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3, glm::vec3 normalLeft, glm::vec3 normalRight,int numLength);
};

#endif // TRIANGLECREATOR_H
