
#ifndef qguirenderpass_h
#define qguirenderpass_h

#include <Render/QGRenderPass.h>
#include <Render/UI/QGUIControl.h>
#include <Render/QGVertexBuffer.h>
#include <Render/QGVertexAttributeList.h>

class QUEST_API QGUIRenderPass : public QGRenderPass {
public:
	QGUIRenderPass() : m_width(0), m_height(0), m_texture(0) {}
	~QGUIRenderPass() = default;

	void Initialize(int width, int height);
	void Render(QGScene* scene);

protected:
	int m_width, m_height;
	QGTexture2D* m_texture;

	// Internal vertex buffer and format for screen pass
	QGVertexBuffer* m_vertexBuffer;
	QGVertexAttributeList* m_vertexFormat;
};

#endif