#ifndef LSCENE_H
#define LSCENE_H

#include "OpenGLScene.h"

#include <memory>
#include "shapes/Shape.h"
#include "l-systems/lnode.h"

namespace CS123 { namespace GL {

    class Shader;
    class CS123Shader;
    class Texture2D;
}}

/**
 *
 * @class LScene
 *
 * A complex scene consisting of multiple objects. Students will implement this class in the
 * Sceneview assignment.
 *
 * Here you will implement your scene graph. The structure is up to you - feel free to create new
 * classes and data structures as you see fit. We are providing this LScene class for you
 * to use as a stencil if you like.
 *
 * Keep in mind that you'll also be rendering entire scenes in the next two assignments, Intersect
 * and Ray. The difference between this assignment and those that follow is here, we are using
 * OpenGL to do the rendering. In Intersect and Ray, you will be responsible for that.
 */
class LScene : public OpenGLScene {
public:
    LScene();
    virtual ~LScene();

    virtual void render(SupportCanvas3D *context) override;
    virtual void settingsChanged() override;

    // Use this method to set an internal selection, based on the (x, y) position of the mouse
    // pointer.  This will be used during the "modeler" lab, so don't worry about it for now.
    void setSelection(int x, int y);
    // The objects in the scene
    std::vector<TreeDistribution> m_sceneObjects;

private:


private:

    void loadPhongShader();
    void setPhongUniforms(SupportCanvas3D *context);
    void setMatrixUniforms(CS123::GL::Shader *shader, SupportCanvas3D *context);
    void setLights();
    void renderGeometry();
    void clearLights();
    void renderPhongPass(SupportCanvas3D *context);
    void renderGeometryAsFilledPolygons();

    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;

};

#endif // LSCENE_H
