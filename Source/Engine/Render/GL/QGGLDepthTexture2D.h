
#ifndef qggldepthtexture2d_h
#define qggldepthtexture2d_h

#include <Render/GL/QGGLTexture2D.h>

class QUEST_API QGGLDepthTexture2D : public QGGLTexture2D {
public:
	QGGLDepthTexture2D() : m_texture(0) {}
	~QGGLDepthTexture2D() = default;

	/**
	 * Use texture
	 */
	void Bind(int slot);

protected:
	unsigned int m_texture;
};

#endif