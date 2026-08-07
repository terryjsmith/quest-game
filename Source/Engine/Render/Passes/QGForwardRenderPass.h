
#ifndef qgforwardrenderpass_h
#define qgforwardrenderpass_h

#include <Render/QGRenderPass.h>

class QUEST_API QGForwardRenderPass : public QGRenderPass {
public:
	QGForwardRenderPass() : m_width(0), m_height(0) {}
	~QGForwardRenderPass() = default;

	void Initialize(int width, int height);
	void Render(QGScene* scene);

protected:
	void RecursiveRender(QGMesh* mesh, matrix4 parentTransform = matrix4(1.0f));

protected:
	int m_width, m_height;
};

#endif