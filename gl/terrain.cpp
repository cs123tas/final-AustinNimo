#include "terrain.h"
#include <iostream>
#include <math.h>
#include "gl/shaders/ShaderAttribLocations.h"
#include <random>
#include <algorithm>
#include <chrono>

Terrain::Terrain() : m_numRows(200), m_numCols(m_numRows)
{
}


/**
 * Returns a pseudo-random value between -1.0 and 1.0 for the given row and column.
 */
float Terrain::randValue(int row, int col) {
    return -1.0 + 2.0 * glm::fract(sin(row * 127.1f + col * 311.7f) * 43758.5453123f);
}

std::vector<glm::vec3> Terrain::getlocations(int num) {
    std::vector<glm::vec3> returnVector;

    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> rows(0,m_numRows);
    std::uniform_int_distribution<int> cols(0,m_numCols);

    int row;
    int col;

    for (int i=0; i<num; i++)
    {
        row = rows(generator);
        col = cols(generator);

        glm::vec3 pos = getPosition(row, col);

        if (std::count(returnVector.begin(), returnVector.end(), pos)) {
            i = i - 1;
        }
        else {
            returnVector.push_back(pos);
            returnVector.push_back(getNormal(row, col));
        }
    }

    return returnVector;
}

float Terrain::getNoiseValue(int row, int col, float octave) {
    float newRow = row/octave;
    float newCol = col/octave;
    float fracRow = glm::fract(newRow);
    float fracCol = glm::fract(newCol);
    float floorRow = glm::floor(newRow);
    float floorCol = glm::floor(newCol);


    float up = glm::mix(randValue(floorRow, floorCol), randValue(floorRow, floorCol + 1), fracCol * fracCol * (3 - 2 *fracCol));
    float down = glm::mix(randValue(floorRow + 1, floorCol), randValue(floorRow + 1, floorCol + 1), fracCol * fracCol * (3 - 2 *fracCol));
    float x =fracRow * fracRow * (3 - 2 *fracRow);
    float result = glm::mix(up,down,x);

    return result;
}

/**
 * Returns the object-space position for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getPosition(int row, int col) {
    glm::vec3 position;
    position.x = 10 * row/m_numRows - 5;
    position.y = 0;
    position.z = 10 * col/m_numCols - 5;

    float oct = 10.0;
    float fact = 2.0;
    float scale = 1.0;
    for(int x=0;x<5;x++){
        float result = getNoiseValue(row,col, oct);
        float lower = 2.00;
        result = glm::pow(result,lower);
        position.y += result;

        oct = oct*fact;
        scale = scale/2.0;
    }

        if(col>150 &&row>150){
        position.y=.25;

    }
    if(col>0 &&col>40 &&row<20){
       position.y=position.y/3;
    }
    return position;
}


/**
 * Returns the normal vector for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getNormal(int row, int col) {
    // TODO: Compute the normal at the given row and column using the positions of the
    //       neighboring vertices.
    glm::vec3 positions[9];
   positions[0] = getPosition(row, col);
   positions[1] = getPosition(row , col + 1);
   positions[2] = getPosition(row - 1, col + 1);
   positions[3] = getPosition(row - 1, col);
   positions[4] = getPosition(row - 1, col - 1);
   positions[5] = getPosition(row, col - 1);
   positions[6] = getPosition(row + 1, col - 1);
   positions[7] = getPosition(row + 1, col);
   positions[8] = getPosition(row + 1, col + 1);

   glm::vec3 vectors[8];

   for (int i = 0; i < 8; i++) {
       vectors[i] = positions[0] - positions[i + 1];
   }

   glm::vec3 normals[8];

   for (int i = 0; i < 7; i++) {
       vectors[i] = glm::cross(positions[i + 1], positions[i]);
   }
   vectors[7] = glm::cross(positions[0], positions[7]);

   float x;
   float y;
   float z;

   for (int i = 0; i < 8; i ++) {
       x += vectors[i].x;
       y += vectors[i].y;
       z += vectors[i].z;
   }

   x = x / 8.0;
   y = y / 8.0;
   z = z / 8.0;
   glm::vec3 result ={x,y,z};

   return glm::normalize(result);

}



struct terrainColor {
    terrainColor(float _height, glm::vec3 _color) {
        height = _height;
        color = _color;
    };
    float height;
    glm::vec3 color;
};

glm::vec3 Terrain::generate_biome(glm::vec3 position, int row, int col) {
    std::vector<glm::vec3> colors;
    std::vector<terrainColor> biomeColors;

    glm::vec3 color = glm::vec3(255/255.0, 255/255.0, 255/255.0);


    biomeColors.push_back(terrainColor(.15, glm::vec3(101/255.0, 67/255.0, 33/255.0)));   // brown for mountain
    biomeColors.push_back(terrainColor(.15, glm::vec3(255/255.0, 255/255.0, 255/255.0)));   // snow for mountain


    if(col>0 &&col>40 &&row<20){
            color = glm::vec3(156/255.0, 211/255.0, 219/255.0);//blue
        }

    else if(position.y<.9){

        color = glm::vec3(101/255.0, 67/255.0, 33/255.0);//brown
    }
    else if(position.y>.9){

        color= glm::vec3(255/255.0, 255/255.0, 255/255.0);//white
    }

    if(col>150 &&row>150){

       color = glm::vec3(132/255.0, 192/255.0, 17/255.0);//green
}

    return color;
}

/**
 * Initializes the terrain by storing positions and normals in a vertex buffer.
 */
void Terrain::init() {
    // TODO: Change from GL_LINE to GL_FILL in order to render full triangles instead of wireframe.

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    std::vector<glm::vec3> colors;

    // Initializes a grid of vertices using triangle strips.
    int numVertices = (m_numRows - 1) * (2 * m_numCols + 2);

    std::vector<float> data(18 * numVertices);
    int index = 0;
    int offset = 0;
   // for(int i =0; i< 3; i++){
    for (int row = 0; row < m_numRows - 1; row++) {
        for (int col = m_numCols - 1; col >= 0; col--) {

            data[index++] = getPosition(row, col).x;//Position
            data[index++] = getPosition(row, col).y;
            data[index++] = getPosition(row, col).z;
            data[index++] = getNormal  (row, col).x;//Normals
            data[index++] = getNormal  (row, col).y;
            data[index++] = getNormal  (row, col).z;

            data[index++] = generate_biome(getPosition(row, col), row, col).x;//Colors
            data[index++] = generate_biome(getPosition(row, col), row, col).y;
            data[index++] = generate_biome(getPosition(row, col), row, col).z;

            data[index++] = getPosition(row, col).x;//UV Coordinates
            data[index++] = getPosition(row, col).y;

            data[index++] = getPosition(row + 1, col).x;//Position
            data[index++] = getPosition(row + 1, col).y;
            data[index++] = getPosition(row + 1, col).z;
            data[index++] = getNormal  (row + 1, col).x;//Normals
            data[index++] = getNormal  (row + 1, col).y;
            data[index++] = getNormal  (row + 1, col).z;

            data[index++] = generate_biome(getPosition(row+1, col),row + 1, col).x;//Colors
            data[index++] = generate_biome(getPosition(row+1, col),row + 1, col).y;
            data[index++] = generate_biome(getPosition(row+1, col),row + 1, col).z;
            data[index++] = getPosition(row + 1, col).x;//UV Coordinates
            data[index++] = getPosition(row + 1, col).y;

        }
        data[index++] = getPosition(row + 1, 0).x;
        data[index++] = getPosition(row + 1, 0).y;
        data[index++] = getPosition(row + 1, 0).z;
        data[index++] = getNormal  (row + 1, 0).x;
        data[index++] = getNormal  (row + 1, 0).y;
        data[index++] = getNormal  (row + 1, 0).z;
        data[index++] = generate_biome(getPosition(row+1, 0),row + 1, 0).x;
        data[index++] = generate_biome(getPosition(row+1, 0),row + 1, 0).y;
        data[index++] = generate_biome(getPosition(row+1, 0),row + 1, 0).z;

        data[index++] = getPosition(row + 1, 0).x;
        data[index++] = getPosition(row + 1, 0).y;
        data[index++] = getPosition(row + 1, m_numCols - 1).x;
        data[index++] = getPosition(row + 1, m_numCols - 1).y;
        data[index++] = getPosition(row + 1, m_numCols - 1).z;
        data[index++] = getNormal  (row + 1, m_numCols - 1).x;
        data[index++] = getNormal  (row + 1, m_numCols - 1).y;
        data[index++] = getNormal  (row + 1, m_numCols - 1).z;

        data[index++] = generate_biome(getPosition(row+1, m_numCols - 1),row + 1, m_numCols - 1).x;
        data[index++] = generate_biome(getPosition(row+1, m_numCols - 1),row + 1, m_numCols - 1).y;
        data[index++] = generate_biome(getPosition(row+1, m_numCols - 1),row + 1, m_numCols - 1).z;
        data[index++] = getPosition(row + 1, m_numCols - 1).x;
        data[index++] = getPosition(row + 1, m_numCols - 1).y;


    }

    // Initialize OpenGLShape.
    m_shape = std::make_unique<OpenGLShape>();

    m_shape->setVertexData(&data[0], data.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, numVertices);
    m_shape->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::NORMAL, 3, sizeof(glm::vec3), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::COLOR, 3, sizeof(glm::vec3)*2, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::TEXCOORD0, 2, sizeof(glm::vec3)*3,VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->buildVAO();


}


/**
 * Draws the terrain.
 */
void Terrain::draw()
{
    m_shape->draw();

}
