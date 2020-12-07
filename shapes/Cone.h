#ifndef CONE_H
#define CONE_H

#include "Shape.h"
#include "TriangleCreator.h"

class Cone : public Shape, TriangleCreator
{
public:
    Cone();
    ~Cone();
    Cone(int param1, int param2);
};

#endif // CONE_H
