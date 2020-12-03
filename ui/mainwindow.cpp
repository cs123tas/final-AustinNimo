#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
#include <QGLFormat>
#include "l-systems/generator.h"
#include <QFileDialog>

const QString DEFAULT_FOLDER = "D:\\Nodal\\Documents\\GitHub\\final-AustinNimo\\l-systems";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect button signal to appropriate slot
    connect(ui->loadLSystemFileButton, &QPushButton::released, this, &MainWindow::loadLSystemFileButton);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadLSystemFileButton() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    DEFAULT_FOLDER,
                                                    tr("Text files (*.txt)"));
    Generator systemGen;
    systemGen.readFile(fileName.toLocal8Bit().data());
}
