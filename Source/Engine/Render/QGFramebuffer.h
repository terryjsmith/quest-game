
#ifndef qgframebuffer_h
#define qgframebuffer_h

#include <Core/QGObject.h>
#include <Render/QGTexture2D.h>

class QUEST_API QGFramebuffer : public QGObject {
public:
	QGFramebuffer() = default;
	virtual ~QGFramebuffer() = default;

    /**
     * Attach a texture to our framebuffer
     */
    virtual void AddTexture(QGTexture2D* texture, int type) {}

    /**
     * Get a texture back out
     */
    QGTexture2D* GetTexture(int type) { return m_textures[type]; }

    /**
     * Use this framebuffer for rendering/output
     */
    virtual void Bind() {}

    /**
     * Clear textures
     */
    void Clear() {
        m_textures.clear();
    }

protected:
    std::map<int, QGTexture2D*> m_textures;
    std::vector<unsigned int> m_slots;
};

#endif