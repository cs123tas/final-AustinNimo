#include "glwidget.h"

//#include "cs123_lib/resourceloader.h"
//#include "cs123_lib/errorchecker.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>

#define PI 3.14159265f

GLWidget::GLWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent), m_angleX(0), m_angleY(0.5f), m_zoom(10.f)
{}

GLWidget::~GLWidget()
{}

void GLWidget::initializeGL() {
    //ResourceLoader::initializeGlew();

    resizeGL(width(), height());


    std::string vertexSource = FileResourceLoader::loadResourceFileToString(":/shaders/terrain.vert");
    std::string fragmentSource = FileResourceLoader::loadResourceFileToString(":/shaders/terrain.frag");
    m_shader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    m_terrain.init();
    initializeGLFragmentShaders();
    rebuildMatrices();
}
void GLWidget::initializeGLFragmentShaders() {

    QImage image(":/shaders/cliff.jpg");

    glGenTextures(1,&m_textureID);
    glBindTexture(GL_TEXTURE_2D,m_textureID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,image.bits());


}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    // Bind shader program.
    //glUseProgram(m_program);

    m_shader->bind();


    m_shader->setUniform("model", glm::value_ptr(m_model));
    m_shader->setUniform("view", glm::value_ptr(m_view));
    m_shader->setUniform("projection", glm::value_ptr(m_projection));

    // Draw terrain.
    m_terrain.draw();

    // Unbind shader program.
    //glUseProgram(0);
    m_shader->unbind();
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    rebuildMatrices();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    m_prevMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    m_angleX += 10 * (event->x() - m_prevMousePos.x()) / (float) width();
    m_angleY += 10 * (event->y() - m_prevMousePos.y()) / (float) height();
    m_prevMousePos = event->pos();
    rebuildMatrices();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    m_zoom -= event->delta() / 100.f;
    rebuildMatrices();
}

void GLWidget::rebuildMatrices() {
    m_model = glm::mat4(1.f);
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
             glm::rotate(m_angleY, glm::vec3(1,0,0)) *
             glm::rotate(m_angleX, glm::vec3(0,1,0));
    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    update();
}
