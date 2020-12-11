#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QButtonGroup>
#include <QMainWindow>
#include "gl/glwidget.h"

class CS123XmlSceneParser;
class SupportCanvas3D;

namespace Ui {
    class MainWindow;
}

class DataBinding;

/**
 * @class MainWindow
 *
 * The main graphical user interface class (GUI class) for our application.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void generateTrees(QString fileName,
                                   std::vector<glm::vec3> treeLocations,
                                   std::vector<glm::vec3> treeAngles,
                                   glm::vec3 treeSize);

    void generateTrees(std::string fileName,
                                   std::vector<glm::vec3> treeLocations,
                                   std::vector<glm::vec3> treeAngles,
                                   glm::vec3 treeSize);
    void clearTrees();

protected:

    // Overridden from QWidget. Handles the window resize event.
    virtual void changeEvent(QEvent *e);

    // Overridden from QWidget. Handles the window close event.
    virtual void closeEvent(QCloseEvent *e);

private:

    // Helper function for checkAllRayFeatures() and uncheckAllRayFeatures(), either check or
    // clear all checkboxes associated with rendering features for Ray, depending on 'checked'.
    void setAllRayFeatures(bool checked);

    // Enables or disables the UI controls.
    void setAllEnabled(bool enabled);

    // Sets up the data bindings between the UI and app settings
    void dataBind();

    // initializes settings and ui for camtrans viewing frustum
    void initializeCamtransFrustum();

    // [C++ Note] private members start with m_
    QList<DataBinding*> m_bindings;
    QList<QButtonGroup*> m_buttonGroups;
    SupportCanvas3D *m_canvas3D;
    std::unique_ptr<CS123XmlSceneParser> m_sceneParser;
    GLWidget *m_glWidget;

    // EXCEPTION: 'ui' member which is auto-generated by Qt. DO NOT RENAME!
    Ui::MainWindow *ui;

public slots:
    // Used internally to keep the aspect ratio up to date.
    void updateAspectRatio();

    // Used internally to keep data bindings and the user interface in sync.
    void settingsChanged();

    // These methods are update different aspects of the 3D camera, and delegate to the Canvas3D.
    void setCameraAxisX();
    void setCameraAxisY();
    void setCameraAxisZ();
    void updateCameraTranslation();
    void updateCameraRotationN();
    void updateCameraRotationV();
    void updateCameraRotationU();
    void resetUpVector();
    void updateCameraClip();
    void updateCameraHeightAngle();
    void setCameraAxonometric();
    void loadLSystemFileButton();
    void generateRandomTreesButton();
    void clearSystemButton();
};

#endif // MAINWINDOW_H
