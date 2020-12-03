#ifndef TORUS_H
#define TORUS_H

#include "Shape.h"
#include "CircleVerticesCreator.h"

class Torus : public Shape, CircleVerticesCreator
{
public:
    Torus();
    ~Torus();
    Torus(int param1, int param2, float param3);
};

#endif // TORUS_H
