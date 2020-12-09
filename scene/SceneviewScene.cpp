#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "camera/Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "lib/FileResourceLoader.h"
#include "gl/shaders/CS123Shader.h"


#include <sstream>


using namespace CS123::GL;


SceneviewScene::SceneviewScene()
{
    // [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    loadPhongShader();
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = FileResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = FileResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::render(SupportCanvas3D *context) {
    setClearColor();

    renderPhongPass(context);
}


void SceneviewScene::renderPhongPass(SupportCanvas3D *context) {
    m_phongShader->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clearLights();
    setLights();
    setPhongUniforms(context);
    setMatrixUniforms(m_phongShader.get(), context);
    renderGeometryAsFilledPolygons();

    m_phongShader->unbind();
}

void SceneviewScene::clearLights() {
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void SceneviewScene::setPhongUniforms(SupportCanvas3D *context) {
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
}

void SceneviewScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    Camera *camera = context->getCamera();

    shader->setUniform("p" , camera->getProjectionMatrix());
    shader->setUniform("v", camera->getViewMatrix());
}

void SceneviewScene::setLights()
{
    //
    // Set up the lighting for your scene using m_phongShader.
    // The lighting information will most likely be stored in CS123SceneLightData structures.
    //
    for (CS123SceneLightData light : m_sceneLights) {
        m_phongShader->setLight(light);
    }
}

void SceneviewScene::renderGeometry() {
    // You shouldn't need to write *any* OpenGL in this class!
    //
    //
    // This is where you should render the geometry of the scene. Use what you
    // know about OpenGL and leverage your Shapes classes to get the job done.
    //
    for (int i = 0; i < (int)m_sceneObjects.size(); i++) {
        CS123SceneMaterial objectMaterial = m_sceneObjects[i]->primitive.material;
        objectMaterial.cAmbient *= m_globalLight.ka;
        objectMaterial.cDiffuse *= m_globalLight.kd;
        m_phongShader->applyMaterial(objectMaterial);
        m_phongShader->setUniform("m", m_sceneObjects[i]->transform);
        m_phongShader->applyTexture(m_sceneObjects[i]->primitive.material.textureMap);
        if (m_sceneObjects[i]->primitive.material.textureMap.isUsed) {
            m_phongShader->setTexture("tex", m_textures[m_sceneObjects[i]->texId]);
            m_sceneObjects[i]->shape->get()->draw();
        } else {
            m_sceneObjects[i]->shape->get()->draw();
        }
    }
}



void SceneviewScene::settingsChanged() {
    buildShapes((int) ceil((float)m_sceneObjects.size() / COMPLEXITY), true);
}

void SceneviewScene::renderGeometryAsFilledPolygons() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
}



