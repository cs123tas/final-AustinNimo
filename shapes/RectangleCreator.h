#ifndef RECTANGLECREATOR_H
#define RECTANGLECREATOR_H

#include "lib/Vertex.h"
#include <vector>

class RectangleCreator
{
public:
    RectangleCreator();
protected:
    std::vector<Vertex> buildRectanglePlane(Vertex vertex1, Vertex vertex2, int numLength, int numWidth, int lengthPlane, int widthPlane1, int widthPlane2);
    std::vector<Vertex> buildRectangleStrip(Vertex vertex1, Vertex vertex2, int numLength, int lengthPlane, int widthPlane1, int widthPlane2, int numWidth);

};

#endif // RECTANGLECREATOR_H
