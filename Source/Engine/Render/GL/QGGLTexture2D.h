
#ifndef qggltexture2d_h
#define qggltexture2d_h

#include <Render/QGTexture2D.h>

class QUEST_API QGGLTexture2D : public QGTexture2D {
public:
	QGGLTexture2D() : m_texture(0) {}
	virtual ~QGGLTexture2D() = default;

	/**
	 * Use texture
	 */
	virtual void Bind(int slot);

	/**
	 * Get texture
	 */
	unsigned int GetTexture() { return m_texture; }

protected:
	unsigned int m_texture;
};

#endif