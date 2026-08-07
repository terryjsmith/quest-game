
#ifndef qgrendersystem_h
#define qgrendersystem_h

#include <Core/QGSystem.h>
#include <Render/QGTexture2D.h>
#include <Render/QGVertexAttributeList.h>
#include <Render/QGIndexBuffer.h>
#include <Render/QGVertexBuffer.h>
#include <Render/QGShader.h>
#include <Render/QGShaderProgram.h>
#include <Render/QGScene.h>
#include <Render/QGRenderPass.h>

class QUEST_API QGRenderSystem : public QGSystem {
public:
	QGRenderSystem();
	~QGRenderSystem() = default;

	/**
	 * Initialize render passes
	 */
	void Initialize(int width, int height);

	/**
	 * Update scene
	 */
	void Update(float delta);

	/**
	 * Draw all of the things on the screen
	 */
	virtual void Render();

	/**
	 * Clear the screen
	 */
	virtual void SetClearColor(vector4 color) {}
	virtual void Clear(int mask) {}

	/**
	 * Draw + indexed draw
	 */
	virtual void Draw(int type, int elements) { }
	virtual void DrawIndexed(int type, int elements) { }

	/**
	 * Set viewport (normally done by framebuffer)
	 */
	virtual void SetViewport(int width, int height) {}

	/**
	 * Depth testing
	 */
	virtual void EnableDepthTest(int type) {}
	virtual void DisableDepthTest() {}

	/**
	 * Create resources
	 */
	virtual QGTexture2D* CreateTexture2D() { return new QGTexture2D(); }
	virtual QGVertexBuffer* CreateVertexBuffer() { return new QGVertexBuffer(); }
	virtual QGIndexBuffer* CreateIndexBuffer() { return new QGIndexBuffer(); }
	virtual QGVertexAttributeList* CreateVertexAttributeList() { return new QGVertexAttributeList(); }
	virtual QGShader* CreateShader() { return new QGShader(); }
	virtual QGShaderProgram* CreateShaderProgram() { return new QGShaderProgram(); }

	/**
	 * Add a render pass
	 */
	void AddRenderPass(int index, QGRenderPass* pass) {
		m_passes[index] = pass;
	}

protected:
	// Scene to be rendered
	QGScene* m_scene;

	// Render passes
	std::map<int, QGRenderPass*> m_passes;
};

#endif