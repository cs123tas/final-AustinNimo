#include "terrain.h"

#include <math.h>
#include "gl/shaders/ShaderAttribLocations.h"
#include <random>
#include <algorithm>
#include <chrono>

Terrain::Terrain() : m_numRows(100), m_numCols(m_numRows)
{
}


/**
 * Returns a pseudo-random value between -1.0 and 1.0 for the given row and column.
 */
float Terrain::randValue(int row, int col) {
    return -1.0 + 2.0 * glm::fract(sin(row * 127.1f + col * 311.7f) * 43758.5453123f);
}

float Terrain::getNoiseValue(int row, int col, float octave) {
    float newRowVal = row / octave;
    float newColVal = col / octave;

    float newRow = glm::floor(newRowVal);
    float newCol = glm::floor(newColVal);

    float fracRow = glm::fract(newRowVal);
    float fracCol = glm::fract(newColVal);

    float topX = glm::mix(randValue(newRow, newCol), randValue(newRow, newCol + 1), fracCol * fracCol * (3 - 2 *fracCol));
    float botX = glm::mix(randValue(newRow + 1, newCol), randValue(newRow + 1, newCol + 1), fracCol * fracCol * (3 - 2 *fracCol));

    float value = glm::mix(topX, botX, fracRow * fracRow * (3 - 2 *fracRow));
    return value;
}

/**
 * Returns the object-space position for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getPosition(int row, int col) {
    glm::vec3 position;
    position.x = 10 * row/m_numRows - 5;
    position.y = 0;
    position.z = 10 * col/m_numCols - 5;




    // TODO: Adjust position.y using value noise.
    //float value = randValue(newRow, newCol);
    position.y += getNoiseValue(row, col, 10.0);
    position.y += getNoiseValue(row, col, 80.0);
    position.y += getNoiseValue(row, col, 40.0);

    return position;
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

    for (int i = 0; i < 7; i++) {
        vectors[i] = glm::cross(positions[i + 1], positions[i]);
    }
    vectors[7] = glm::cross(positions[0], positions[7]);

    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

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


/**
 * Initializes the terrain by storing positions and normals in a vertex buffer.
 */
void Terrain::init() {
    // TODO: Change from GL_LINE to GL_FILL in order to render full triangles instead of wireframe.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // Initializes a grid of vertices using triangle strips.
    int numVertices = (m_numRows - 1) * (2 * m_numCols + 2);
    std::vector<glm::vec3> data(2 * numVertices);
    int index = 0;
    for (int row = 0; row < m_numRows - 1; row++) {
        for (int col = m_numCols - 1; col >= 0; col--) {
            data[index++] = getPosition(row, col);
            data[index++] = getNormal  (row, col);
            data[index++] = getPosition(row + 1, col);
            data[index++] = getNormal  (row + 1, col);
        }
        data[index++] = getPosition(row + 1, 0);
        data[index++] = getNormal  (row + 1, 0);
        data[index++] = getPosition(row + 1, m_numCols - 1);
        data[index++] = getNormal  (row + 1, m_numCols - 1);
    }

    // Initialize OpenGLShape.
    m_shape = std::make_unique<OpenGLShape>();
    m_shape->setVertexData(&data[0][0], data.size() * 3, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, numVertices);
    m_shape->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::NORMAL, 3, sizeof(glm::vec3), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->buildVAO();
}


/**
 * Draws the terrain.
 */
void Terrain::draw()
{
    m_shape->draw();
}
