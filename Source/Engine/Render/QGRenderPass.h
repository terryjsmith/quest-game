
#ifndef qgrenderpass_h
#define qgrenderpass_h

#include <Render/QGScene.h>
#include <Render/QGShaderProgram.h>

class QUEST_API QGRenderPass : public QGObject {
public:
	QGRenderPass() : m_program(0) {}
	virtual ~QGRenderPass() = default;

	virtual void Initialize(int width, int height) = 0;
	virtual void Render(QGScene* scene) = 0;

protected:
	QGShaderProgram* m_program;
};

#endif