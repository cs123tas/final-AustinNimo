#include "CircleVerticesCreator.h"
#include "lib/Vertex.h"
#include <math.h>
#include "glm.hpp"

CircleVerticesCreator::CircleVerticesCreator()
{

}

CircleVerticesCreator::~CircleVerticesCreator()
{

}

// Return a vector of vertex objects by building each slice and then adding the top and bottom vertices
std::vector<Vertex> CircleVerticesCreator::buildSphereVertices(int numHeight, int numWidth, float radius) {

    const glm::vec3 top = {0, radius, 0};
    const glm::vec3 bottom = {0, -radius, 0};
    std::vector<Vertex> vertexInfo;
    Vertex currentVertex = {top,{0,0}, {0,-1,0}};
    float currentTheta;
    float currentPhi = 0;
    float thetaSlice = 2 * M_PI / numWidth;
    float phiSlice = M_PI / numHeight;
    for (int i = 0; i < numHeight - 1; i++) {
        currentTheta = 0;
        currentPhi += phiSlice;
        currentVertex.XYZ.y = radius * cos(currentPhi);
        for (int j = 0; j < numWidth; j++) {
            currentVertex.XYZ.x = radius * sin(currentPhi) * cos(currentTheta);
            currentVertex.XYZ.z = radius * sin(currentPhi) * sin(currentTheta);
            currentVertex.XYZN = glm::normalize(currentVertex.XYZ);
            float theta = atan2(currentVertex.XYZ.z, currentVertex.XYZ.x);
            if (theta < 0) {
                currentVertex.UV.x = -theta / (2.0*M_PI);
            } else {
                currentVertex.UV.x = 1.0 - (theta / (2.0*M_PI));
            }
            float phi = asin(currentVertex.XYZ.y/.5);
            currentVertex.UV.y =  phi / M_PI + .5;
            vertexInfo.push_back(currentVertex);
            currentTheta += thetaSlice;
        }
        currentTheta = 0;
        currentVertex.XYZ.x = radius * sin(currentPhi) * cos(currentTheta);
        currentVertex.XYZ.z = radius * sin(currentPhi) * sin(currentTheta);
        currentVertex.XYZN = glm::normalize(currentVertex.XYZ);
        currentVertex.UV.x = 0.0;
        float phi = asin(currentVertex.XYZ.y/.5);
        currentVertex.UV.y =  phi / M_PI + .5;
        vertexInfo.push_back(currentVertex);


    }
    float widthPercentage = 1.0 / numWidth;
    for (int j = 0; j <= numWidth; j++) {
        currentVertex = {top, {-1,-1}, {0,1,0}};
        currentVertex.UV.x =  1 - (widthPercentage * j);
        currentVertex.UV.y =  1;
        vertexInfo.push_back(currentVertex);
    }
    for (int j = 0; j <= numWidth; j++) {
        currentVertex = {bottom, {-1,-1}, {0,-1,0}};
        currentVertex.UV.x =  1 - (widthPercentage * j);
        currentVertex.UV.y =  0;
        vertexInfo.push_back(currentVertex);
    }

    return vertexInfo;
}

std::vector<Vertex> CircleVerticesCreator::buildTorusVertices(int numWidth, int numHeight, float radius, float radiusPercentage) {

    const glm::vec3 top = {0, radius, 0};
    float torusRadius = radius * radiusPercentage * .01;
    const glm::vec3 bottom = {0, -radius, 0};
    std::vector<Vertex> vertexInfo;
    float currentTorusTheta = 0;
    float currentLargeTheta = 0;
    float largeThetaSlice = 2 * M_PI / numWidth;
    float torusThetaSlice = 2 * M_PI / numHeight;

    // Get the center of the torus on the X/Z plane
    // Rotate this center around the X/Z plans using the radius
    // Then draw an entire circle of points around this on the plane intersecting the origin
    for (int i = 0; i < numWidth; i++) {
        currentTorusTheta = 0;
        glm::vec3 center = {radius * cos(currentLargeTheta), radius * sin(currentLargeTheta), 0};
        for (int j = 0; j < numHeight; j++) {

            glm::vec3 currentPosition = {(radius + torusRadius * cos(currentTorusTheta)) * cos(currentLargeTheta),
                                     (radius + torusRadius * cos(currentTorusTheta)) * sin(currentLargeTheta),
                                     (torusRadius * sin(currentTorusTheta))};
            glm::vec3 currentNormals = glm::normalize(glm::vec3(currentPosition.x - center.x, currentPosition.y - center.y, currentPosition.z - center.z));
            Vertex currentVertex = {currentPosition, {-1,-1}, currentNormals};
            vertexInfo.push_back(currentVertex);
            currentTorusTheta += torusThetaSlice;
        }
        currentLargeTheta += largeThetaSlice;
    }

    return vertexInfo;
}
