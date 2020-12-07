#ifndef SCENE_H
#define SCENE_H

#include "lib/CS123SceneData.h"
#include <glm.hpp>
#include <glm/gtx/transform.hpp>
#include "shapes/Shape.h"
#include <qgl.h>
#include "gl/textures/Texture2D.h"
#include "gl/textures/TextureParametersBuilder.h"
#include <unordered_map>
#include <string.h>
#include "math.h"
#include "lib/SupportMethods.h"

class Camera;
class CS123ISceneParser;

struct SceneObject
{
    CS123ScenePrimitive primitive;
    glm::mat4x4 transform;
    std::unique_ptr<Shape> *shape;
    int texId;
    int texWidth;
    int texHeight;
    std::string imageName;

    SceneObject(){}
    SceneObject(SceneObject *orig){
        primitive = orig->primitive;
        transform = orig->transform;
        shape = orig->shape;
        texId = orig->texId;
        texWidth = orig->texWidth;
        imageName = orig->imageName;
    }

    SceneObject(CS123ScenePrimitive prim) {
        primitive = prim;
    }

    void rotate(float angle, glm::vec3 rotation) {
        transform = glm::rotate(transform, angle, rotation) * transform;
    }
    void translate(glm::vec3 translation) {
        transform = glm::translate(transform, translation);
    }
    void scale(glm::vec3 scale) {
        transform = glm::scale(transform, scale);
    }
    void matrix(glm::mat4x4 matrix) {
        transform = matrix * transform;
    }

};

struct CubeSceneObject : public SceneObject {

    CubeSceneObject(CS123ScenePrimitive prim) {
        primitive = prim;
    }
};

struct ConeSceneObject : public SceneObject {

    ConeSceneObject(CS123ScenePrimitive prim) {
        primitive = prim;
    }
};


struct CylinderSceneObject : public SceneObject {

    CylinderSceneObject(CS123ScenePrimitive prim) {
        primitive = prim;
    }
};


struct SphereSceneObject : public SceneObject {

    SphereSceneObject(CS123ScenePrimitive prim) {
        primitive = prim;
    }
};

/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

    // The objects in the scene
    std::vector<std::shared_ptr<SceneObject>> m_sceneObjects;

    CS123SceneGlobalData m_globalLight;

    // The lights in the scene
    std::vector<CS123SceneLightData> m_sceneLights;
protected:
    CS123::GL::Texture2D loadTexture(CS123SceneFileMap texMap, GLuint texId);

    // Adds the primitives to the scene.
    virtual void addPrimitives();

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    void buildShapes(float divider, bool adjustment);

    // Parse one node recursively
    static std::vector<std::shared_ptr<SceneObject>> parseNode(CS123SceneNode *node, glm::mat4x4 transform);



    // The shapes in the scene
    std::vector<std::unique_ptr<Shape>> m_shapes;
    std::unique_ptr<Shape> m_cube;
    std::unique_ptr<Shape> m_cone;
    std::unique_ptr<Shape> m_torus;
    std::unique_ptr<Shape> m_sphere;
    std::unique_ptr<Shape> m_cylinder;

    int m_comp1;
    int m_comp2;
    int m_comp3;

    const float COMPLEXITY = 20.0;

    std::vector<CS123::GL::Texture2D> m_textures;

    std::unordered_map<std::string, CS123::GL::Texture2D> m_textureMap;


};

#endif // SCENE_H
