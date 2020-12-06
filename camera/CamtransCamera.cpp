/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>

CamtransCamera::CamtransCamera()
{
    setClip(1.0, 30.0);
    setAspectRatio(1.0);
    setHeightAngle(60.0);
    orientLook({2,2,2,0},{-1,-1,-1,0},{0,1,0,0});
}

void CamtransCamera::setAspectRatio(float a)
{
    m_aspectRatio = a;
    updateProjectionMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    glm::mat4x4 result = m_perspectiveTransformation * m_scaleMatrix;
    return result;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    return m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const {
    return -m_w;
}

glm::vec4 CamtransCamera::getUp() const {
    return m_up;
}


glm::vec4 CamtransCamera::getU() const {
    return m_u;
}
glm::vec4 CamtransCamera::getW() const {

    return m_w;
}
glm::vec4 CamtransCamera::getV() const {

    return m_v;
}

float CamtransCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    return m_thetaH;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    m_eye = eye;
    m_w = glm::normalize(-look);
    m_up = up;
    m_v = glm::normalize(up - (glm::dot(up, m_w) * m_w));

    glm::vec3 v3 = {m_v.x,m_v.y,m_v.z};
    glm::vec3 w3 = {m_w.x,m_w.y,m_w.z};
    glm::vec3 u3 = glm::cross(v3, w3);
    m_u = {u3.x, u3.y, u3.z, 0.0};
    m_u = glm::normalize(m_u);
    updateViewMatrix();
}

void CamtransCamera::setHeightAngle(float h) {
    float radians = glm::radians(h);
    m_thetaH = radians;
    float height = tan(radians / 2.0) * m_far * 2.0;
    float w = m_aspectRatio * height;
    m_thetaW = w / 2.0 / m_far;
    updateProjectionMatrix();

}

void CamtransCamera::translate(const glm::vec4 &v) {
    m_eye = m_eye + v;
    updateViewMatrix();
}

//void CamtransCamera::rotateU(float degrees) {
//    float radians = glm::radians(degrees);

//    m_v = {m_w.x * sin(radians) + m_v.x * cos(radians),
//           m_w.y * sin(radians) + m_v.y * cos(radians),
//           m_w.z * sin(radians) + m_v.z * cos(radians), 0.0};
//    m_w = {m_w.x * cos(radians) - m_v.x * sin(radians),
//           m_w.y * cos(radians) - m_v.y * sin(radians),
//           m_w.z * cos(radians) - m_v.z * sin(radians), 0.0};
//    m_v = glm::normalize(m_v);
//    m_w = glm::normalize(m_w);
//    updateRotationMatrix();


//}

//void CamtransCamera::rotateV(float degrees) {
//    float radians = glm::radians(degrees);

//    m_u = {m_u.x * cos(radians) - m_w.x * sin(radians),
//           m_u.y * cos(radians) - m_w.y * sin(radians),
//           m_u.z * cos(radians) - m_w.z * sin(radians), 0.0};
//    m_w = {m_u.x * sin(radians) + m_w.x * cos(radians),
//           m_u.y * sin(radians) + m_w.y * cos(radians),
//           m_u.z * sin(radians) + m_w.z * cos(radians), 0.0};

//    m_u = glm::normalize(m_u);
//    m_w = glm::normalize(m_w);
//    updateRotationMatrix();


//}

//void CamtransCamera::rotateW(float degrees) {
//    float radians = -glm::radians(degrees);

//    m_u = {m_u.x * cos(radians) - m_v.x * sin(radians),
//           m_u.y * cos(radians) - m_v.y * sin(radians),
//           m_u.z * cos(radians) - m_v.z * sin(radians), 0.0};
//    m_v = {m_u.x * sin(radians) + m_v.x * cos(radians),
//           m_u.y * sin(radians) + m_v.y * cos(radians),
//           m_u.z * sin(radians) + m_v.z * cos(radians), 0.0};
//    m_u = glm::normalize(m_u);
//    m_v = glm::normalize(m_v);
//    updateRotationMatrix();
//}

void CamtransCamera::rotateU(float degrees) {
    float radians = glm::radians(degrees);
    float sinRadians = glm::sin(radians);
    float cosRadians = glm::cos(radians);

    m_v = {m_w.x * sinRadians + m_v.x * cosRadians,
           m_w.y * sinRadians + m_v.y * cosRadians,
           m_w.z * sinRadians + m_v.z * cosRadians, 0.0};
    m_w = {m_w.x * cosRadians - m_v.x * sinRadians,
           m_w.y * cosRadians - m_v.y * sinRadians,
           m_w.z * cosRadians - m_v.z * sinRadians, 0.0};
    m_v = glm::normalize(m_v);
    m_w = glm::normalize(m_w);
    updateRotationMatrix();


}

void CamtransCamera::rotateV(float degrees) {
    float radians = glm::radians(degrees);
    float sinRadians = glm::sin(radians);
    float cosRadians = glm::cos(radians);

    m_u = {m_u.x * cosRadians - m_w.x * sinRadians,
           m_u.y * cosRadians - m_w.y * sinRadians,
           m_u.z * cosRadians - m_w.z * sinRadians, 0.0};
    m_w = {m_u.x * sinRadians + m_w.x * cosRadians,
           m_u.y * sinRadians + m_w.y * cosRadians,
           m_u.z * sinRadians + m_w.z * cosRadians, 0.0};

    m_u = glm::normalize(m_u);
    m_w = glm::normalize(m_w);
    updateRotationMatrix();


}

void CamtransCamera::rotateW(float degrees) {
    float radians = -glm::radians(degrees);
    float sinRadians = glm::sin(radians);
    float cosRadians = glm::cos(radians);

    m_u = {m_u.x * cosRadians - m_v.x * sinRadians,
           m_u.y * cosRadians - m_v.y * sinRadians,
           m_u.z * cosRadians - m_v.z * sinRadians, 0.0};
    m_v = {m_u.x * sinRadians + m_v.x * cosRadians,
           m_u.y * sinRadians + m_v.y * cosRadians,
           m_u.z * sinRadians + m_v.z * cosRadians, 0.0};
    m_u = glm::normalize(m_u);
    m_v = glm::normalize(m_v);
    updateRotationMatrix();
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}

void CamtransCamera::updateProjectionMatrix() {
    updateScaleMatrix();
    updatePerspectiveMatrix();
}
void CamtransCamera::updatePerspectiveMatrix() {
    m_perspectiveTransformation = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0/(-m_near/m_far + 1.0), -1.0, 0.0, 0.0, (-m_near/m_far)/(-m_near/m_far + 1.0), 0.0};
}
void CamtransCamera::updateScaleMatrix() {
    m_scaleMatrix = {(1.0/m_thetaW)/m_far, 0.0, 0.0, 0.0, 0.0, (1.0/tan(m_thetaH/2.0))/m_far, 0.0, 0.0, 0.0, 0.0, 1.0/m_far, 0.0, 0.0, 0.0, 0.0, 1.0};
}
void CamtransCamera::updateViewMatrix() {
    updateTranslationMatrix();
    updateRotationMatrix();

}
void CamtransCamera::updateRotationMatrix() {
    m_rotationMatrix = {m_u.x, m_v.x, m_w.x, 0.0, m_u.y, m_v.y, m_w.y, 0.0, m_u.z, m_v.z, m_w.z, 0.0, 0.0, 0.0, 0.0, 1.0};

}
void CamtransCamera::updateTranslationMatrix() {
    m_translationMatrix = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -m_eye.x, -m_eye.y, -m_eye.z, 1.0};
}

