#include "RectangleCreator.h"

RectangleCreator::RectangleCreator()
{


}

std::vector<Vertex> RectangleCreator::buildRectanglePlane(Vertex vertex1, Vertex vertex2, int numLength, int numWidth, int lengthPlane, int widthPlane1, int widthPlane2)
{
    std::vector<Vertex> vertexInfo;
    vertexInfo.clear();
    Vertex startVertex = vertex1;
    Vertex endVertex = vertex1;
    endVertex.XYZ[lengthPlane] = vertex2.XYZ[lengthPlane];
    endVertex.XYZN = vertex2.XYZN;
    float stripWidth1 = (vertex2.XYZ[widthPlane1] - vertex1.XYZ[widthPlane1]) / numWidth;
    float stripWidth2 = (vertex2.XYZ[widthPlane2] - vertex1.XYZ[widthPlane2]) / numWidth;
    for (int i = 0; i < numWidth; i++) {
        endVertex.XYZ[widthPlane1] += stripWidth1;
        endVertex.XYZ[widthPlane2] += stripWidth2;
        std::vector<Vertex> stripVertices = buildRectangleStrip(startVertex, endVertex, numLength, lengthPlane, widthPlane1, widthPlane2, numWidth);
        vertexInfo.insert(vertexInfo.end(), stripVertices.begin(), stripVertices.end());
        vertexInfo.push_back(SKIP_VERTEX);
        startVertex.XYZ[widthPlane1] = endVertex.XYZ[widthPlane1];
        startVertex.XYZ[widthPlane2] = endVertex.XYZ[widthPlane2];
    }
    return vertexInfo;
}

std::vector<Vertex> RectangleCreator::buildRectangleStrip(Vertex vertex1, Vertex vertex2, int numLength, int lengthPlane, int widthPlane1, int widthPlane2, int numWidth)
{
    std::vector<Vertex> vertexInfo;
    vertexInfo.clear();
    Vertex currentVertex = vertex1;
    // Counter clockwise
    //currentVertex.XYZ[widthPlane] = vertex2.XYZ[widthPlane];
    float triangleLength = (vertex2.XYZ[lengthPlane] - vertex1.XYZ[lengthPlane]) / numLength;
    for (int i = 0; i <= numLength; i++) {
        vertexInfo.push_back(currentVertex);
        currentVertex.XYZ[widthPlane1] = vertex2.XYZ[widthPlane1];
        currentVertex.XYZ[widthPlane2] = vertex2.XYZ[widthPlane2];
        currentVertex.XYZN = vertex2.XYZN;
        vertexInfo.push_back(currentVertex);
        currentVertex.XYZ[lengthPlane] += triangleLength;
        currentVertex.XYZ[widthPlane1] = vertex1.XYZ[widthPlane1];
        currentVertex.XYZ[widthPlane2] = vertex1.XYZ[widthPlane2];
        currentVertex.XYZN = vertex1.XYZN;
    }
    return vertexInfo;
}
