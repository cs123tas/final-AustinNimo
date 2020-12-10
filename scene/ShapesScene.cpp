#include "ShapesScene.h"
#include "camera/Camera.h"
#include "shapes/Shape.h"
#include <SupportCanvas3D.h>
#include <QFileDialog>

#include <sstream>

#include "shapes/ExampleShape.h"
#include "shapes/Cube.h"
#include "shapes/Cone.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include "shapes/Torus.h"
#include "shapes/LoadedMesh.h"

using namespace CS123::GL;
#include "gl/shaders/CS123Shader.h"
#include "gl/shaders/Shader.h"

#include "lib/FileResourceLoader.h"

ShapesScene::ShapesScene(int width, int height) :
    m_meshVertices(),
    m_meshIndices(),
    m_shape(nullptr),
    m_width(width),
    m_height(height)
{
    initializeSceneMaterial();
    initializeSceneLight();
    loadPhongShader();

}

ShapesScene::~ShapesScene()
{
    // Pro-tip: If you use smart pointers properly, this destructor should be empty
}

void ShapesScene::initializeSceneMaterial() {
    // Use a shiny orange material
    m_material.clear();
    m_material.cAmbient.r = 0.2f;
    m_material.cAmbient.g = 0.1f;
    m_material.cDiffuse.r = 1.0f;
    m_material.cDiffuse.g = 0.5f;
    m_material.cSpecular.r = m_material.cSpecular.g = m_material.cSpecular.b = 1;
    m_material.shininess = 64;
}

void ShapesScene::initializeSceneLight() {
    // Use a white directional light from the upper left corner
    memset(&m_light, 0, sizeof(m_light));
    m_light.type = LightType::LIGHT_DIRECTIONAL;
    m_light.dir = m_lightDirection;
    m_light.color.r = m_light.color.g = m_light.color.b = 1;
    m_light.id = 0;
}

void ShapesScene::loadPhongShader() {
    std::string vertexSource = FileResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = FileResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void ShapesScene::render(SupportCanvas3D *context) {
    // Clear the screen in preparation for the next frame.
    setClearColor();

    renderPhongPass(context);
}

void ShapesScene::renderPhongPass(SupportCanvas3D *context) {
    m_phongShader->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clearLights();
    setLights(context->getCamera()->getViewMatrix());
    setPhongSceneUniforms();
    setMatrixUniforms(m_phongShader.get(), context);
    renderGeometryAsFilledPolygons();

    m_phongShader->unbind();
}

void ShapesScene::setPhongSceneUniforms() {
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->applyMaterial(m_material);
}

void ShapesScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
    shader->setUniform("m", glm::mat4(1.0f));
}

void ShapesScene::renderGeometryAsFilledPolygons() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
}

void ShapesScene::renderGeometry() {
    // [SHAPES] Render the shape. Lab 1 seems like it'll come in handy...
    if (m_shape) {
        m_shape->draw();
    }
}

void ShapesScene::clearLights() {
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void ShapesScene::loadMesh(std::vector<Vertex> vertices, std::vector<int> indices) {
    m_meshVertices = vertices;
    m_meshIndices = indices;
    settings.shapeType = SHAPE_SPECIAL_1;
    settingsChanged();
}

void ShapesScene::setLights(const glm::mat4 viewMatrix) {
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    m_light.dir = glm::inverse(viewMatrix) * m_lightDirection;

    clearLights();
    m_phongShader->setLight(m_light);
}

void ShapesScene::settingsChanged() {
    // [SHAPES] Fill this in, for now default to an example shape
    switch(settings.shapeType) {
        case SHAPE_CUBE:
            m_shape = std::make_unique<Cube>(settings.shapeParameter1);
            break;

        case SHAPE_CONE:
        m_shape = std::make_unique<Cone>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_SPHERE:
        m_shape = std::make_unique<Sphere>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_CYLINDER:
        m_shape = std::make_unique<Cylinder>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_TORUS:
        m_shape = std::make_unique<Torus>(settings.shapeParameter1, settings.shapeParameter2, settings.shapeParameter3);
            break;
        case SHAPE_SPECIAL_1:
        if (m_meshIndices.size() > 0) {
            m_shape = std::make_unique<LoadedMesh>(m_meshVertices, m_meshIndices);
        } else {
            m_shape = nullptr;
        }
            break;
        case SHAPE_SPECIAL_2:
            m_shape = nullptr;
            break;
    }
}

