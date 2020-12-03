#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include "TriangleCreator.h"
#include "RectangleCreator.h"

class Cylinder : public Shape, TriangleCreator, RectangleCreator
{
public:
    Cylinder();
    ~Cylinder();
    Cylinder(int param1, int param2);
};

#endif // CYLINDER_H
