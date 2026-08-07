
#ifndef qgglrendersystem_h
#define qgglrendersystem_h

#include <Render/QGRenderSystem.h>
#include <Render/GL/QGGLIndexBuffer.h>
#include <Render/GL/QGGLShaderProgram.h>
#include <Render/GL/QGGLTexture2D.h>
#include <Render/GL/QGGLVertexBuffer.h>
#include <Render/GL/QGGLVertexAttributeList.h>

class QUEST_API QGGLRenderSystem : public QGRenderSystem {
public:
	QGGLRenderSystem() = default;
	~QGGLRenderSystem() = default;

	/**
	 * Initialize OpenGL
	 */
	void InitializeGL();

	/**
	 * Clear buffers
	 */
	void SetClearColor(vector4 color);
	void Clear(int mask);

	/**
	 * Draw + indexed draw
	 */
	void Draw(int type, int elements);
	void DrawIndexed(int type, int elements);

	/**
	 * Set viewport (normally done by framebuffer)
	 */
	void SetViewport(int width, int height);

	/**
	 * Depth testing
	 */
	void EnableDepthTest(int type);
	void DisableDepthTest();

	/**
	 * Create resources
	 */
	QGTexture2D* CreateTexture2D() { return new QGGLTexture2D(); }
	QGVertexBuffer* CreateVertexBuffer() { return new QGGLVertexBuffer(); }
	QGIndexBuffer* CreateIndexBuffer() { return new QGGLIndexBuffer(); }
	QGVertexAttributeList* CreateVertexAttributeList() { return new QGGLVertexAttributeList(); }
	QGShaderProgram* CreateShaderProgram() { return new QGGLShaderProgram(); }
};

#endif