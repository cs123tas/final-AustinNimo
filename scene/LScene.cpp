#include "LScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "camera/Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "lib/ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "shapes/Cylinder.h"

#include <sstream>

//TODO Allow editing the trunk color
CS123SceneColor TREE_TRUNK_BROWN(0.324,0.207,0.039,1.0);

using namespace CS123::GL;


LScene::LScene()
{
    // [SCENEVIEW] Set up anything you need for your Sceneview scene here..
    CS123SceneLightData light;
    light.id = 0;
    light.type = LightType::LIGHT_DIRECTIONAL;
    light.dir = {0.0,0.0,-1.0,0.0};
    light.color = {1.0,1.0,1.0,1.0};
    m_sceneLights.push_back(light);
    loadPhongShader();
}

LScene::~LScene()
{
}

void LScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void LScene::render(SupportCanvas3D *context) {
    setClearColor();

    renderPhongPass(context);
}


void LScene::renderPhongPass(SupportCanvas3D *context) {
    m_phongShader->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    clearLights();
    setLights();
    setPhongUniforms(context);
    setMatrixUniforms(m_phongShader.get(), context);
    renderGeometryAsFilledPolygons();

    m_phongShader->unbind();
}

void LScene::clearLights() {
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void LScene::setPhongUniforms(SupportCanvas3D *context) {
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
}

void LScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    Camera *camera = context->getCamera();

    shader->setUniform("p" , camera->getProjectionMatrix());
    shader->setUniform("v", camera->getViewMatrix());
}

void LScene::setLights()
{
    //
    // Set up the lighting for your scene using m_phongShader.
    // The lighting information will most likely be stored in CS123SceneLightData structures.
    //
    for (CS123SceneLightData light : m_sceneLights) {
        m_phongShader->setLight(light);
    }
}

void LScene::renderGeometry() {
    // You shouldn't need to write *any* OpenGL in this class!
    //
    //
    // This is where you should render the geometry of the scene. Use what you
    // know about OpenGL and leverage your Shapes classes to get the job done.
    //

    // TODO ALlow materials to switch as part of the created object
    for (int i = 0; i < (int)m_sceneObjects.size(); i++) {
        CS123SceneMaterial objectMaterial = CS123SceneMaterial();
        objectMaterial.cAmbient = TREE_TRUNK_BROWN;
        //objectMaterial.cAmbient = TREE_TRUNK_BROWN;
        objectMaterial.cDiffuse = TREE_TRUNK_BROWN;
        // TODO Fix lighting
        //objectMaterial.cAmbient *= m_globalLight.ka;
        //objectMaterial.cDiffuse *= m_globalLight.kd;
        m_phongShader->applyMaterial(objectMaterial);
        m_phongShader->setUniform("m", m_sceneObjects[i]->transform);

        // TODO Add textures back if there's time
//        m_phongShader->applyTexture(m_sceneObjects[i]->primitive.material.textureMap);
//        if (m_sceneObjects[i]->primitive.material.textureMap.isUsed) {
//            m_phongShader->setTexture("tex", m_textures[m_sceneObjects[i]->texId]);
//            m_sceneObjects[i]->shape->get()->draw();
//        } else {
//            m_sceneObjects[i]->shape->get()->draw();
//        }
        m_sceneObjects[i].get()->shape.get()->draw();
        }
}



void LScene::settingsChanged() {
    buildShapes((int) ceil((float)m_sceneObjects.size() / COMPLEXITY), true);
}

void LScene::renderGeometryAsFilledPolygons() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
}



