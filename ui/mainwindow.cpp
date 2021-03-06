#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Databinding.h"
#include "SupportCanvas3D.h"
#include "lib/CS123XmlSceneParser.h"
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>
#include "shapes/MeshLoader.h"
#include "lib/Vertex.h"
#include "l-systems/generator.h"
#include "Settings.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>

const QString DEFAULT_FOLDER = "D:\\Nodal\\Documents\\GitHub\\final-AustinNimo\\l-systems";
const int NUM_TREES = 20;

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

//    QWidget *widget = ui->tabWidget->currentWidget();
//    ui->tabWidget->setCurrentWidget(ui->tab3D);
//    show();
//    ui->tabWidget->setCurrentWidget(widget);
    show();
    // Connect button signal to appropriate slot
     connect(ui->loadLSystemFileButton, &QPushButton::released, this, &MainWindow::loadLSystemFileButton);
     connect(ui->clearSystemButton, &QPushButton::released, this, &MainWindow::clearSystemButton);
     connect(ui->generateRandomTreesButton, &QPushButton::released, this, &MainWindow::generateRandomTreesButton);

}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings)
        delete b;
    foreach (QButtonGroup *bg, m_buttonGroups)
        delete bg;
    delete ui;
}

std::vector<float> parseFloats(std::string s) {
    std::vector<float> vect;

    std::stringstream ss(s);

    float i;

    while (ss >> i)
    {


        vect.push_back(i);
        if (ss.peek() == ',' || ss.peek() == '\n'  || ss.peek() == ' ') {
            ss.ignore();
        }
    }
    return vect;
}

std::vector<glm::vec3> parseVecs(std::vector<float> nums) {
    std::vector<glm::vec3> returnVec;
    for (int i = 0; i < (int)nums.size(); i = i + 3) {
        glm::vec3 newVec = glm::vec3(nums[i], nums[i+1], nums[i+2]);
        returnVec.push_back(newVec);
    }
    return returnVec;
}

void MainWindow::loadLSystemFileButton() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    DEFAULT_FOLDER,
                                                    tr("Text files (*.txt)"));
    if (fileName != "") {

        std::vector<float> locations = parseFloats(
                    ui->treeLocs->toPlainText().toUtf8().constData());
        std::vector<float> angles = parseFloats(
                    ui->treeAngles->toPlainText().toUtf8().constData());
        std::vector<float> sizes = parseFloats(
                    ui->treeSizes->toPlainText().toUtf8().constData());

        if (locations.size() == angles.size() && locations.size() % 3 == 0 && sizes.size() == 3) {
            std::vector<glm::vec3> locationVec = parseVecs(locations);
            std::vector<glm::vec3> angleVec = parseVecs(angles);
            glm::vec3 sizeVec = parseVecs(sizes)[0];

            generateTrees(std::string(fileName.toUtf8().data()), locationVec, angleVec, sizeVec);

        }
    }
}



void MainWindow::clearSystemButton() {
    clearTrees();
}

QString getTreeFile(int num) {
    switch(num % 5) {
    case 0: {
        return QString(":/l-systems/l-systems/OakFall.txt");
        break;
    }
    case 1: {
        return QString(":/l-systems/l-systems/OakSummer.txt");
        break;
    }
    case 2: {
        return QString(":/l-systems/l-systems/OakWinter.txt");
        break;
    }
    case 3: {
        return QString(":/l-systems/l-systems/ScarletMaple.txt");
        break;
    }
    case 4: {
        return QString(":/l-systems/l-systems/Sakura.txt");
        break;
    }
    }
    return "";
}

void MainWindow::generateRandomTreesButton() {

    // Generate positions for trees
    std::vector<glm::vec3> treeLocations = m_canvas3D->m_terrain.getlocations(NUM_TREES);
    std::vector<glm::vec3> treeLocs;
    std::vector<glm::vec3> treeAngles;
    for(int i = 0; i < 5; i++) {
        treeLocs.clear();
        treeAngles.clear();
        int index = i * 2;
        while (index < NUM_TREES * 2) {
            std::cout << index << " loc " << treeLocations[index].x <<":"<< treeLocations[index].y <<":"<< treeLocations[index].z <<
                         " ang " << treeLocations[index + 1].x << ":"  << treeLocations[index + 1].y << ":"  << treeLocations[index + 1].z << std::endl;
            treeLocs.push_back(treeLocations[index]);
//            treeAngles.push_back(treeLocations[index + 1]);
            treeAngles.push_back({0.0,1.0,0.0});
            // 5 types of trees, locs and angles
            index += 5 * 2;
        }
        generateTrees(getTreeFile(i), treeLocs, treeAngles, {.1,.1,.1});
    }
}

void MainWindow::generateTrees(QString fileName,
                               std::vector<glm::vec3> treeLocations,
                               std::vector<glm::vec3> treeAngles,
                               glm::vec3 treeSize) {
    Generator systemGen;

    TreeDistribution newTrees;
    newTrees.treeNodes = systemGen.readFile(fileName, {0.0, 1.0, 0.0}, {0.0,0.0,0.0}, treeSize);
    newTrees.treeAngles = treeAngles;
    newTrees.treeLocations = treeLocations;

    glm::mat4x4 model;
    glm::vec3 targetVec;
    glm::quat rotQuat;
    glm::mat4 rotationMatrix;
    for(int i = 0; i < (int) newTrees.treeLocations.size(); i++) {
        model = glm::translate(glm::mat4(), newTrees.treeLocations[i]);
        targetVec = newTrees.treeAngles[i];
        rotQuat = glm::rotation({0.0,1.0,0.0}, targetVec);
        rotationMatrix = glm::toMat4(rotQuat);
        model = model * rotationMatrix;
        newTrees.treeModels.push_back(model);
    }
    m_canvas3D->m_lScene.get()->m_sceneObjects.push_back(newTrees);
}

void MainWindow::generateTrees(std::string fileName,
                               std::vector<glm::vec3> treeLocations,
                               std::vector<glm::vec3> treeAngles,
                               glm::vec3 treeSize) {
    Generator systemGen;

    TreeDistribution newTrees;
    newTrees.treeNodes = systemGen.readFile(fileName, {0.0, 1.0, 0.0}, {0.0,0.0,0.0}, treeSize);
    newTrees.treeAngles = treeAngles;
    newTrees.treeLocations = treeLocations;

    glm::mat4x4 model;
    glm::vec3 targetVec;
    glm::quat rotQuat;
    glm::mat4 rotationMatrix;
    for(int i = 0; i < (int) newTrees.treeLocations.size(); i++) {
        model = glm::translate(glm::mat4(), newTrees.treeLocations[i]);
        targetVec = newTrees.treeAngles[i];
        rotQuat = glm::rotation({0.0,1.0,0.0}, targetVec);
        rotationMatrix = glm::toMat4(rotQuat);
        model = model * rotationMatrix;
        newTrees.treeModels.push_back(model);
    }
    m_canvas3D->m_lScene.get()->m_sceneObjects.push_back(newTrees);
}

void MainWindow::clearTrees() {
    m_canvas3D->m_lScene.get()->m_sceneObjects.clear();
    m_canvas3D->repaint();
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

    BIND(BoolBinding::bindCheckbox(ui->useLightingCheckbox, settings.useLighting))

#undef BIND
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

void MainWindow::setAllEnabled(bool enabled) {
    QList<QWidget *> widgets;

    QList<QAction *> actions;

    foreach (QWidget *widget, widgets)
        widget->setEnabled(enabled);
    foreach (QAction *action, actions)
        action->setEnabled(enabled);
}
