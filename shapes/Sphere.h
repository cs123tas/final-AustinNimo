#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "CircleVerticesCreator.h"

class Sphere : public Shape, CircleVerticesCreator
{
public:
    Sphere();
    ~Sphere();
    Sphere(int param1, int param2);
};

#endif // SPHERE_H
