
#ifndef qggltexture2d_h
#define qggltexture2d_h

#include <Render/QGTexture2D.h>

class QUEST_API QGGLTexture2D : public QGTexture2D {
public:
	QGGLTexture2D() : m_texture(0) {}
	~QGGLTexture2D() = default;

	/**
	 * Use texture
	 */
	void Bind(int slot);

protected:
	unsigned int m_texture;
};

#endif