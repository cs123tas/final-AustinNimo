#include "Texture2D.h"

#include <utility>

namespace CS123 { namespace GL {

Texture2D::Texture2D(unsigned char *data, int width, int height, GLenum type)
{
    GLenum internalFormat = type == GL_FLOAT ? GL_RGBA32F : GL_RGBA;

    // [Task 2]
    // Bind the texture by calling bind() and filling it in
    // Generate the texture with glTexImage2D

        bind();

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat, GL_UNSIGNED_BYTE, data);

    // Don't forget to unbind!
        unbind();
}

//TODO is this enough?
Texture2D::Texture2D(Texture2D *tex) {
    m_handle = tex->m_handle;
}

void Texture2D::bind() const {
    // [Task 2]
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture2D::unbind() const {
    // Don't forget to unbind!
    glBindTexture(GL_TEXTURE_2D, 0);
}

}}
