
#ifndef qggbufferpass_h
#define qggbufferpass_h

#include <Render/QGRenderPass.h>

class QUEST_API QGGBufferPass : public QGRenderPass {
public:
	QGGBufferPass() {
		m_animatedMeshProgram = 0;

		m_positionTexture = 0;
		m_normalTexture = 0;
		m_diffuseTexture = 0;

		m_width = m_height = 0;
	}
	~QGGBufferPass() = default;

	void Initialize(int width, int height);
	void Render(QGScene* scene);

protected:
	void RecursiveRender(QGMesh* mesh, bool animated, QGShaderProgram* program, matrix4 parentTransform = matrix4(1.0f));

protected:
	QGShaderProgram* m_animatedMeshProgram;

	// Our textures
	QGTexture2D* m_positionTexture;
	QGTexture2D* m_normalTexture;
	QGTexture2D* m_diffuseTexture;
	QGTexture2D* m_depthTexture;

	int m_width, m_height;
};

#endif