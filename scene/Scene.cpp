#include "Scene.h"
#include "camera/Camera.h"
#include "lib/CS123ISceneParser.h"
#include "lib/CS123SceneData.h"

#include "glm/gtx/transform.hpp"
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "shapes/Cylinder.h"
#include "shapes/LoadedMesh.h"
#include "Settings.h"
#include "lib/Vertex.h"
#include <QFileDialog>
#include <memory>

using namespace CS123::GL;

Scene::Scene()
{
}

Scene::Scene(Scene &scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(global);

    // [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.
    // TODO make sure a deep copy isn't needed.
    m_sceneLights = scene.m_sceneLights;
    m_globalLight = scene.m_globalLight;
    m_sceneObjects = scene.m_sceneObjects;
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D

}

void Scene::buildShapes(float divider, bool adjustment = false) {
    int comp1 = (int)ceil((float)settings.shapeParameter1 / divider);
    int comp2 = (int)ceil((float)settings.shapeParameter2 / divider);
    int comp3 = (int)ceil((float)settings.shapeParameter3 / divider);
    if (!adjustment or comp1 != m_comp1 or comp2 != m_comp2 or comp3 != m_comp3) {
        m_comp1 = comp1;
        m_comp2 = comp2;
        m_comp3 = comp3;

        m_cylinder = std::make_unique<Cylinder>(comp1, comp2);
        m_cylinder->buildVAO();
    }
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()

    // Get the root node and parse
    CS123SceneNode* root = parser->getRootNode();
    glm::mat4x4 transform;

    std::vector<std::shared_ptr<SceneObject>> parsedSceneObjects = parseNode(root, transform);
    sceneToFill->m_sceneObjects = std::move(parsedSceneObjects);
    sceneToFill->addPrimitives();


    // Get the lights and parse.
    int numLights = parser->getNumLights();
    for(int i = 0; i < numLights; i++) {
        CS123SceneLightData lightData;
        parser->getLightData(i, lightData);
        sceneToFill->addLight(lightData);
    }

    // Get the global data and parse
    CS123SceneGlobalData globalData;
    parser->getGlobalData(globalData);
    sceneToFill->setGlobal(globalData);

}

std::vector<std::shared_ptr<SceneObject>> Scene::parseNode(CS123SceneNode *node, glm::mat4x4 transform) {
    std::vector<CS123SceneNode*> children = node->children;
    std::vector<std::shared_ptr<SceneObject>> returnObjects;

    // Apply transforms
    for (CS123SceneTransformation *csTransform : node->transformations) {
        switch(csTransform->type) {
            case TRANSFORMATION_TRANSLATE:
                transform = glm::translate(transform, csTransform->translate);
                break;
            case TRANSFORMATION_SCALE:
                transform = glm::scale(transform, csTransform->scale);
                break;
            case TRANSFORMATION_ROTATE:
                transform = glm::rotate(transform, csTransform->angle, csTransform->rotate);
                break;
            case TRANSFORMATION_MATRIX:
                transform = csTransform->matrix * transform;
                break;
        }
    }

    for (CS123SceneNode* child : children) {
        std::vector<std::shared_ptr<SceneObject>> childObjects = parseNode(child, transform);
        for(int i = 0; i < (int)childObjects.size();i++){
            returnObjects.push_back(std::move(childObjects[i]));
        }
    }
    for (CS123ScenePrimitive *prim : node->primitives) {
        std::shared_ptr<SceneObject> newObject;
        switch(prim->type) {
            case PrimitiveType::PRIMITIVE_CUBE:
                newObject = std::make_unique<CubeSceneObject>(*prim);
                break;
            case PrimitiveType::PRIMITIVE_CONE:
            newObject = std::make_unique<ConeSceneObject>(*prim);
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
            newObject = std::make_unique<CylinderSceneObject>(*prim);
                break;
            case PrimitiveType::PRIMITIVE_SPHERE:
            newObject = std::make_unique<SphereSceneObject>(*prim);
                break;
            default:
                break;
        }

        newObject->transform = transform;
        returnObjects.push_back(std::move(newObject));
    }

    return returnObjects;
}

void Scene::addPrimitives() {
    // Save a pointer to the shape to save memory
    for (int i = 0; i < (int)m_sceneObjects.size(); i++) {
        switch(m_sceneObjects[i]->primitive.type) {
            case PrimitiveType::PRIMITIVE_CUBE:
                m_sceneObjects[i]->shape = &m_cube;
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                m_sceneObjects[i]->shape = &m_cone;
                break;
            case PrimitiveType::PRIMITIVE_MESH:
                break;
            case PrimitiveType::PRIMITIVE_TORUS:
                m_sceneObjects[i]->shape = &m_torus;
                break;
            case PrimitiveType::PRIMITIVE_SPHERE:
                m_sceneObjects[i]->shape = &m_sphere;
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                m_sceneObjects[i]->shape = &m_cylinder;
                break;
        }
        if (m_sceneObjects[i]->primitive.material.textureMap.isUsed) {
            // Load in the texture and save it
            QImage image = QImage(m_sceneObjects[i]->primitive.material.textureMap.filename.data());
            QImage fImage = QGLWidget::convertToGLFormat(image);
            Texture2D texture(fImage.bits(),fImage.width(),fImage.height());
            TextureParametersBuilder builder;
            builder.setFilter(TextureParameters::FILTER_METHOD::NEAREST);
            builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
            TextureParameters parameters = builder.build();
            parameters.applyTo(texture);
            m_textures.push_back(std::move(texture));
            m_sceneObjects[i]->texId = m_textures.size() - 1;
            //Texture2D tex = loadTexture(parsedSceneObjects[i].primitive.material.textureMap,parsedSceneObjects[i].texId);

        } else {
            m_sceneObjects[i]->texId = -1;
        }
    }
    buildShapes(ceil(m_sceneObjects.size() / COMPLEXITY));

}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_sceneLights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_globalLight = global;
}
