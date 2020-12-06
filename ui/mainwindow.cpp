#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Databinding.h"
#include "SupportCanvas3D.h"
#include "lib/CS123XmlSceneParser.h"
#include "scene/ShapesScene.h"
#include "scene/SceneviewScene.h"
#include "camera/CamtransCamera.h"
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>
#include "shapes/MeshLoader.h"
#include "lib/Vertex.h"
#include "l-systems/generator.h"

const QString DEFAULT_FOLDER = "D:\\Nodal\\Documents\\GitHub\\final-AustinNimo\\l-systems";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Make sure the settings are loaded before the UI
    settings.loadSettingsOrDefaults();

    QGLFormat qglFormat;
    qglFormat.setVersion(4, 3);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setSampleBuffers(true);
    ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(ui->canvas3D);
    m_canvas3D = new SupportCanvas3D(qglFormat, this);
    gridLayout->addWidget(m_canvas3D, 0, 1);
    ui->tabWidget->setCurrentWidget(ui->tab3D);

    // Restore the UI settings
    QSettings qtSettings("CS123", "CS123");
    restoreGeometry(qtSettings.value("geometry").toByteArray());
    restoreState(qtSettings.value("windowState").toByteArray());

    // Resize the window because the window is huge since all docks were visible.
    resize(1000, 600);

    // Bind the UI elements to their properties in the global Settings object, using binding
    // objects to simplify the code.  Each binding object connects to its UI elements and keeps
    // the UI and its setting in sync.

    QList<QAction*> actions;

#define SETUP_ACTION(dock, key) \
    actions.push_back(ui->dock->toggleViewAction()); \
    actions.back()->setShortcut(QKeySequence(key));

    ui->menuToolbars->addActions(actions);
#undef SETUP_ACTION


    dataBind();

    QList<QRadioButton*> a;

    a.clear();
    a += ui->shapeTypeCone;
    a += ui->shapeTypeCube;
    a += ui->shapeTypeCylinder;
    a += ui->shapeTypeTorus;
    a += ui->shapeTypeSpecial1;
    a += ui->shapeTypeSpecial2;
    foreach (QRadioButton *rb, a)
        connect(rb, SIGNAL(clicked()), this, SLOT(activateCanvas3D()));

    QWidget *widget = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->tab3D);
    show();
    ui->tabWidget->setCurrentWidget(widget);
    show();
    // Connect button signal to appropriate slot
     connect(ui->loadLSystemFileButton, &QPushButton::released, this, &MainWindow::loadLSystemFileButton);

}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings)
        delete b;
    foreach (QButtonGroup *bg, m_buttonGroups)
        delete bg;
    delete ui;
}

void MainWindow::loadLSystemFileButton() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    DEFAULT_FOLDER,
                                                    tr("Text files (*.txt)"));
    Generator systemGen;
    systemGen.readFile(fileName.toLocal8Bit().data());
    // TODO make this cleaner
    m_canvas3D->m_lScene.get()->m_sceneObjects = systemGen.m_shapeNodes;
}

void MainWindow::dataBind() {
    // Brush dock
#define BIND(b) { \
    DataBinding *_b = (b); \
    m_bindings.push_back(_b); \
    assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); \
}
    QButtonGroup *brushButtonGroup = new QButtonGroup;
    QButtonGroup *shapesButtonGroup = new QButtonGroup;
    QButtonGroup *filterButtonGroup = new QButtonGroup;
    m_buttonGroups.push_back(brushButtonGroup);
    m_buttonGroups.push_back(shapesButtonGroup);
    m_buttonGroups.push_back(filterButtonGroup);

    BIND(ChoiceBinding::bindRadioButtons(
            shapesButtonGroup,
            NUM_SHAPE_TYPES,
            settings.shapeType,
            ui->shapeTypeCube,
            ui->shapeTypeCone,
            ui->shapeTypeSphere,
            ui->shapeTypeCylinder,
            ui->shapeTypeTorus,
            ui->shapeTypeSpecial1,
            ui->shapeTypeSpecial2))
    BIND(IntBinding::bindSliderAndTextbox(
        ui->shapeParameterSlider1, ui->shapeParameterTextbox1, settings.shapeParameter1, 1.f, 100.f))
    BIND(IntBinding::bindSliderAndTextbox(
        ui->shapeParameterSlider2, ui->shapeParameterTextbox2, settings.shapeParameter2, 1.f, 100.f))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->shapeParameterSlider3, ui->shapeParameterTextbox3, settings.shapeParameter3, 1.f, 100.f))
    BIND(BoolBinding::bindCheckbox(ui->useLightingCheckbox, settings.useLighting))

    BIND(ChoiceBinding::bindTabs(ui->tabWidget, settings.currentTab))

#undef BIND

    // make sure the aspect ratio updates when m_canvas3D changes size
    connect(m_canvas3D, SIGNAL(aspectRatioChanged()), this, SLOT(updateAspectRatio()));
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e); // allow the superclass to handle this for the most part...

    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Save the settings before we quit
    settings.saveSettings();
    QSettings qtSettings("CS123", "CS123");
    qtSettings.setValue("geometry", saveGeometry());
    qtSettings.setValue("windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::updateAspectRatio() {
    // Update the aspect ratio text so the aspect ratio will be correct even if the
    // 3D canvas isn't visible (the 3D canvas isn't resized when it isn't visible)
//    QSize activeTabSize = ui->tabWidget->currentWidget()->size();
//    float aspectRatio = static_cast<float>(activeTabSize.width()) / static_cast<float>(activeTabSize.height());
}


void MainWindow::settingsChanged() {
    m_canvas3D->settingsChanged();
}

void MainWindow::fileOpen() {
    // This opens the 3D tab to initialize OGL so parsing
    // the scene doesn't crash. If you can find a better solution
    // feel free to change this.
    activateCanvas3D();
    QString file = QFileDialog::getOpenFileName(this, QString(), "/course/cs123/data/");
    if (!file.isNull()) {
        if (file.endsWith(".xml")) {
            CS123XmlSceneParser parser(file.toLatin1().data());
            if (parser.parse()) {
                m_canvas3D->loadSceneviewSceneFromParser(parser);

                // Set the camera for the new scene
                CS123SceneCameraData camera;
                if (parser.getCameraData(camera)) {
                    camera.pos[3] = 1;
                    camera.look[3] = 0;
                    camera.up[3] = 0;

                    CamtransCamera *cam = m_canvas3D->getCamtransCamera();
                    cam->orientLook(camera.pos, camera.look, camera.up);
                    cam->setHeightAngle(camera.heightAngle);
                }

                activateCanvas3D();
            } else {
                QMessageBox::critical(this, "Error", "Could not load scene \"" + file + "\"");
            }
        } else if (file.endsWith(".obj")) {
            MeshLoader loader;
            loader.loadMesh(file.toLatin1().data());
            std::vector<Vertex> vertices = loader.getVertices();
            std::vector<int> indices = loader.getIndices();
            m_canvas3D->loadMesh(vertices, indices);
            ui->shapeTypeSpecial1->setChecked(true);

        }
    }
}

void MainWindow::setAllEnabled(bool enabled) {
    QList<QWidget *> widgets;

    QList<QAction *> actions;
    actions += ui->actionNew;
    actions += ui->actionOpen;
    actions += ui->actionSave;
    actions += ui->actionRevert;
    actions += ui->actionCopy3Dto2D;
    actions += ui->actionClear;
    actions += ui->actionQuit;

    foreach (QWidget *widget, widgets)
        widget->setEnabled(enabled);
    foreach (QAction *action, actions)
        action->setEnabled(enabled);
}

void MainWindow::activateCanvas3D() {
    ui->tabWidget->setCurrentWidget(ui->tab3D);
}

void MainWindow::setCameraAxisX() {
    m_canvas3D->setCameraAxisX();
}

void MainWindow::setCameraAxisY() {
    m_canvas3D->setCameraAxisY();
}

void MainWindow::setCameraAxisZ() {
    m_canvas3D->setCameraAxisZ();
}

void MainWindow::updateCameraTranslation() {
    m_canvas3D->updateCameraTranslation();
}

void MainWindow::updateCameraRotationN() {
    m_canvas3D->updateCameraRotationN();
}

void MainWindow::updateCameraRotationV() {
    m_canvas3D->updateCameraRotationV();
}

void MainWindow::updateCameraRotationU() {
    m_canvas3D->updateCameraRotationU();
}

void MainWindow::resetUpVector() {
    m_canvas3D->resetUpVector();
}

void MainWindow::updateCameraClip() {
    m_canvas3D->updateCameraClip();
}

void MainWindow::updateCameraHeightAngle() {
    m_canvas3D->updateCameraHeightAngle();
}

void MainWindow::setCameraAxonometric() {
    m_canvas3D->setCameraAxonometric();
}