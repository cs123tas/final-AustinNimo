#include "TriangleCreator.h"

TriangleCreator::TriangleCreator()
{

}

std::vector<Vertex> TriangleCreator::buildTriangleStrip(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3, glm::vec3 normalLeft, glm::vec3 normalRight,int numLength) {

    glm::vec3 slopeVertexLeft = vertex2;
    slopeVertexLeft -= vertex1;
    slopeVertexLeft /= numLength;
    glm::vec3 slopeVertexRight = vertex3;
    slopeVertexRight -= vertex1;
    slopeVertexRight /= numLength;
    Vertex currentSlice = {vertex1, {-1,-1}, {(normalLeft.x + normalRight.x) / 2, (normalLeft.y + normalRight.y) / 2, (normalLeft.z + normalRight.z) / 2}};
    std::vector<Vertex> vertexInfo = {currentSlice};
    glm::vec3 nextLeft = currentSlice.XYZ;
    glm::vec3 nextRight = currentSlice.XYZ;


    for (int i = 0; i < numLength; i++) {
       nextLeft = nextLeft + slopeVertexLeft;
       nextRight = nextRight + slopeVertexRight;
       currentSlice = {nextRight, {-1,-1}, normalRight};
       vertexInfo.push_back(currentSlice);
       currentSlice = {nextLeft, {-1,-1}, normalLeft};
       vertexInfo.push_back(currentSlice);
    }
    vertexInfo.push_back(SKIP_VERTEX);

    return vertexInfo;
}
