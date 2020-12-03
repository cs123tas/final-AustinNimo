#ifndef CUBE_H
#define CUBE_H
#include "Shape.h"
#include "RectangleCreator.h"

// hand-written cube points and normals.. if only there were a way to do this procedurally
#define CUBE_DATA_POSITIONS {\
        {{-0.5f,-0.5f,-0.5f}, \
        {-1.f, 0.f, 0.f}}, \
        {{-0.5f,-0.5f, 0.5f},\
        {-1.f, 0.f, 0.f}}, \
        {{-0.5f, 0.5f, 0.5f}, \
        {-1.f, 0.f, 0.f}}, \
        {{0.5f, 0.5f,-0.5f}, \
        {0.f, 0.f, -1.f}}, \
        {{-0.5f,-0.5f,-0.5f},\
        {0.f, 0.f, -1.f}}, \
        {{-0.5f, 0.5f,-0.5f}, \
        {0.f, 0.f, -1.f}}, \
        {{0.5f,-0.5f, 0.5f}, \
        {0.f, -1.f, 0.f}}, \
        {{-0.5f,-0.5f,-0.5f}, \
        {0.f, -1.f, 0.f}}, \
        {{0.5f,-0.5f,-0.5f}, \
        {0.f, -1.f, 0.f}}, \
        {{0.5f, 0.5f,-0.5f}, \
        {0.f, 0.f, -1.f}}, \
        {{0.5f,-0.5f,-0.5f}, \
        {0.f, 0.f, -1.f}}, \
        {{-0.5f,-0.5f,-0.5f}, \
        {0.f, 0.f, -1.f}}, \
        {{-0.5f,-0.5f,-0.5f}, \
        {-1.f, 0.f, 0.f}}, \
        {{-0.5f, 0.5f, 0.5f},\
        {-1.f, 0.f, 0.f}}, \
        {{-0.5f, 0.5f,-0.5f},\
        {-1.f, 0.f, 0.f}}, \
        {{0.5f,-0.5f, 0.5f},\
        {0.f, -1.f, 0.f}}, \
        {{-0.5f,-0.5f, 0.5f},\
        {0.f, -1.f, 0.f}}, \
        {{-0.5f,-0.5f,-0.5f},\
        {0.f, -1.f, 0.f}}, \
        {{-0.5f, 0.5f, 0.5f},\
        {0.f, 0.f, 1.f}}, \
        {{-0.5f,-0.5f, 0.5f},\
        {0.f, 0.f, 1.f}}, \
        {{0.5f,-0.5f, 0.5f},\
        {0.f, 0.f, 1.f}}, \
        {{0.5f, 0.5f, 0.5f},\
        {1.f, 0.f, 0.f}}, \
        {{0.5f,-0.5f,-0.5f},\
        {1.f, 0.f, 0.f}}, \
        {{0.5f, 0.5f,-0.5f},\
        {1.f, 0.f, 0.f}}, \
        {{0.5f,-0.5f,-0.5f},\
        {1.f, 0.f, 0.f}}, \
        {{0.5f, 0.5f, 0.5f},\
        {1.f, 0.f, 0.f}}, \
        {{0.5f,-0.5f, 0.5f},\
        {1.f, 0.f, 0.f}}, \
        {{0.5f, 0.5f, 0.5f},\
        {0.f, 1.f, 0.f}}, \
        {{0.5f, 0.5f,-0.5f},\
        {0.f, 1.f, 0.f}}, \
        {{-0.5f, 0.5f,-0.5f},\
        {0.f, 1.f, 0.f}}, \
        {{0.5f, 0.5f, 0.5f},\
        {0.f, 1.f, 0.f}}, \
        {{-0.5f, 0.5f,-0.5f},\
        {0.f, 1.f, 0.f}}, \
        {{-0.5f, 0.5f, 0.5f},\
        {0.f, 1.f, 0.f}}, \
        {{0.5f, 0.5f, 0.5f},\
        {0.f, 0.f, 1.f}}, \
        {{-0.5f, 0.5f, 0.5f},\
        {0.f, 0.f, 1.f}}, \
        {{0.5f,-0.5f, 0.5f}, \
        {0.f, 0.f, 1.f}}}

#define Indices {\
    0, 1, 2,\
    3, 4, 5,\
    6, 7, 8,\
    9, 10, 11,\
    12, 13, 14,\
    15, 16, 17,\
    18, 19, 20,\
    21, 22, 23,\
    24, 25, 26,\
    27, 28, 29,\
    30, 31, 32,\
    33, 34, 35}

class Cube : public Shape, RectangleCreator
{
public:
    Cube();
    ~Cube();
    Cube(int param1);

private:
    int m_param1;
//    std::vector<Vertex> buildRectanglePlane(Vertex vertex1, Vertex vertex2, int numLength, int numWidth, int lengthPlane, int widthPlane);
//    std::vector<Vertex> buildRectangleStrip(Vertex vertex1, Vertex vertex2, int numLength, int lengthPlane, int widthPlane);

};



#endif // CUBE_H
