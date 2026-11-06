
#ifndef qgglframebuffer_h
#define qgglframebuffer_h

#include <Render/QGFramebuffer.h>

class QUEST_API QGGLFramebuffer : public QGFramebuffer {
public:
    QGGLFramebuffer() : m_complete(false), m_framebuffer(0) {}
	~QGGLFramebuffer() = default;

    /**
     * Attach a texture to our framebuffer
     */
    virtual void AddTexture(QGTexture2D* texture, int type);

    /**
     * Use this framebuffer for rendering/output
     */
    virtual void Bind();

protected:
    unsigned int m_framebuffer;
    bool m_complete;
};

#endif