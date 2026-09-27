
#include <Render/Passes/QGUIRenderPass.h>
#include <Render/UI/QGUISystem.h>
#include <Render/QGRenderSystem.h>
#include <IO/QGResourceSystem.h>
#include <Core/QGApplication.h>

void QGUIRenderPass::Initialize(int width, int height) {
	m_width = width;
	m_height = height;

	QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();
	if (m_program == 0) {
		m_program = renderSystem->CreateShaderProgram();
		m_program->vertexShader = (QGShader*)resourceSystem->Load("Resources/Shaders/ortho.vs", "Shader");
		m_program->fragmentShader = (QGShader*)resourceSystem->Load("Resources/Shaders/ortho.fs", "Shader");
	}

	// Populate our vertex buffer and type
	float box[] = {
		(float)width, 0, 1, 1,
		0, 0, 0, 1,
		(float)width, (float)height, 1, 0,
		0, (float)height, 0, 0,
	};

	m_vertexFormat = renderSystem->CreateVertexAttributeList();
	m_vertexFormat->AddVertexAttribute(QGVertexAttribute::ATTRIB_POSITION, 2, 0);
	m_vertexFormat->AddVertexAttribute(QGVertexAttribute::ATTRIB_TEXCOORD0, 2, 2);

	m_vertexBuffer = renderSystem->CreateVertexBuffer();
	m_vertexBuffer->Create(m_vertexFormat, 4, box, false);
}

void QGUIRenderPass::Render(QGScene* scene) {
	QGUISystem* uiSystem = GetQGSystem<QGUISystem>();
	if (uiSystem == 0) return; // If not initialized, shouldn't be here, but return

	// If no currently active screen, also return
	if (uiSystem->ActiveScreen() == 0) return;

	// Otherwise, draw!
	uiSystem->Render(m_width, m_height);

	// After render, send to texture
	if (m_texture == 0) {
		QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();
		m_texture = renderSystem->CreateTexture2D();
	}

	unsigned char* data = uiSystem->Data();
	m_texture->Create(m_width, m_height, 4, QGTexture2D::QGTEXTURE_BYTE, data);

	// Need to actually render to screen here
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();

	// Set viewport
	renderSystem->SetViewport(m_width, m_height);

	// Disable depth testing
	renderSystem->DisableDepthTest();

	// Bind shader program
	m_program->Bind();

	// Get matrices
	matrix4 proj = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);

	m_program->Set("ortho", proj);

	int vertexCount = m_vertexBuffer->Count();

	m_vertexFormat->Bind();
	m_vertexBuffer->Bind();

	// Enable the attributes we need
	m_vertexFormat->EnableAttribute(0, QGVertexAttribute::ATTRIB_POSITION);
	m_vertexFormat->EnableAttribute(3, QGVertexAttribute::ATTRIB_TEXCOORD0);

	// Bind textures
	m_texture->Bind(0);
	m_program->Set("inputTexture", 0);

	renderSystem->Draw(DRAW_TRIANGLE_STRIP, vertexCount);
}
