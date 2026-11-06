
#ifdef USE_OPENGL

#include <Render/GL/QGGLFramebuffer.h>
#include <Render/GL/QGGLTexture2D.h>
#include <Render/GL/QGGL.h>

void QGGLFramebuffer::AddTexture(QGTexture2D* texture, int type) {
    m_textures[type] = texture;

    // Don't add depth textures to our framebuffer slots/counts
    if (type == GL_DEPTH_ATTACHMENT) {
        return;
    }

    m_slots.push_back(type);
    m_complete = false;
}

void QGGLFramebuffer::Bind() {
    if (m_complete == false) {
        if (m_framebuffer == 0) {
            // Initialize our framebuffer
            GL_CHECK(glGenFramebuffers(1, &m_framebuffer));
            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer));
        }

        for (auto it = m_textures.begin(); it != m_textures.end(); it++) {
            // Attach the texture to the FBO
            QGGLTexture2D* gltexture = (QGGLTexture2D*)it->second;
            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, it->first, GL_TEXTURE_2D, gltexture->GetTexture(), 0));
        }

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        QGASSERT(status != GL_FRAMEBUFFER_COMPLETE, "Unable to attach framebuffer.");

        m_complete = true;
    }

    QGTexture2D* first = (QGTexture2D*)m_textures[0];
    GL_CHECK(glViewport(0, 0, first->width, first->height));
}

#endif