
#include <Render/Passes/QGForwardRenderPass.h>
#include <IO/QGResourceSystem.h>
#include <Core/QGApplication.h>
#include <Render/QGRenderSystem.h>
#include <Render/QGRenderDefines.h>
#include <Core/QGEntity.h>
#include <Render/QGAnimatedMeshComponent.h>

void QGForwardRenderPass::Initialize(int width, int height) {
	m_width = width;
	m_height = height;

	QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();
	if (m_program == 0) {
		m_program = renderSystem->CreateShaderProgram();
		m_program->vertexShader = (QGShader*)resourceSystem->Load("Resources/Shaders/forward.vs", "Shader");
		m_program->fragmentShader = (QGShader*)resourceSystem->Load("Resources/Shaders/forward.fs", "Shader");
	}

	if (m_animatedMeshProgram == 0) {
		m_animatedMeshProgram = renderSystem->CreateShaderProgram();
		m_animatedMeshProgram->vertexShader = (QGShader*)resourceSystem->Load("Resources/Shaders/animated.vs", "Shader");
		m_animatedMeshProgram->fragmentShader = (QGShader*)resourceSystem->Load("Resources/Shaders/forward.fs", "Shader");
	}
}

void QGForwardRenderPass::Render(QGScene* scene) {
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();

	// Set viewport
	renderSystem->SetViewport(m_width, m_height);

	// Clear screen
	renderSystem->Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

	// Enable depth testing
	renderSystem->EnableDepthTest(TEST_LESS);

	// Bind shader program
	m_program->Bind();

	// Set camera matrices
	QGCameraComponent* camera = scene->camera;

	camera->aspectRatio = (float)m_width / m_height;
	m_program->Set("projectionMatrix", camera->ProjectionMatrix());
	m_program->Set("viewMatrix", camera->ViewMatrix());

	// Render all of the items in the scene
	auto mit = scene->meshes.begin();
	for (; mit != scene->meshes.end(); mit++) {
		// Skip animated meshes for now
		if (dynamic_cast<QGAnimatedMeshComponent*>(*mit) != 0) continue;

		QGMeshComponent* meshc = (*mit);
		QGMesh* mesh = meshc->mesh;

		QGEntity* entity = meshc->Entity();
		matrix4 transform = entity->transform.Matrix();
		this->RecursiveRender(mesh, false, m_program, transform);
	}

	// Repeat for animated meshes
	m_animatedMeshProgram->Bind();

	m_animatedMeshProgram->Set("projectionMatrix", camera->ProjectionMatrix());
	m_animatedMeshProgram->Set("viewMatrix", camera->ViewMatrix());

	mit = scene->meshes.begin();
	for (; mit != scene->meshes.end(); mit++) {
		// Only animated meshes now
		if (dynamic_cast<QGAnimatedMeshComponent*>(*mit) == 0) continue;

		QGAnimatedMeshComponent* meshc = dynamic_cast<QGAnimatedMeshComponent*>(*mit);
		QGMesh* mesh = meshc->mesh;

		// Set bone matrices
		int counter = 0;
		for (auto bit = meshc->boneMatrix.begin(); bit != meshc->boneMatrix.end(); bit++) {
			QGASSERT(counter < 100, "Overflow - too many bones.");
			m_animatedMeshProgram->Set("finalBoneMatrices[" + std::to_string(counter) + "]", bit->second);
			counter++;
		}

		QGEntity* entity = meshc->Entity();
		matrix4 transform = entity->transform.Matrix();
		this->RecursiveRender(mesh, true, m_animatedMeshProgram, transform);
	}
}

void QGForwardRenderPass::RecursiveRender(QGMesh* mesh, bool animated, QGShaderProgram* program, matrix4 parentTransform) {
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();
	matrix4 modelMatrix = parentTransform * mesh->transform.Matrix();

	if (mesh->children.size() > 0) {
		// recurse instead
		for (auto it = mesh->children.begin(); it != mesh->children.end(); it++) {
			this->RecursiveRender(*it, animated, program, modelMatrix);
		}
		return;
	}

	//modelMatrix = matrix4(1.0f);
	program->Set("modelMatrix", modelMatrix);

	// Bind vertex buffer and layout
	QGVertexAttributeList* attribListObject = mesh->vertexBuffer->VertexAttributeList();
	attribListObject->Bind();
	mesh->vertexBuffer->Bind();

	// Bind index buffer (if applicable)
	if (mesh->indexBuffer) {
		mesh->indexBuffer->Bind();
	}

	// Enable vertex attributes
	attribListObject->EnableAttribute(0, QGVertexAttribute::ATTRIB_POSITION);
	attribListObject->EnableAttribute(1, QGVertexAttribute::ATTRIB_COLOR);
	attribListObject->EnableAttribute(2, QGVertexAttribute::ATTRIB_TEXCOORD0);
	if (animated) {
		attribListObject->EnableAttribute(3, QGVertexAttribute::ATTRIB_BONES);
		attribListObject->EnableAttribute(4, QGVertexAttribute::ATTRIB_BONEWEIGHTS);
	}

	// Bind textures
	if (mesh->diffuseTexture) {
		mesh->diffuseTexture->Bind(0);
		program->Set("diffuseTexture", 0);
	}

	if (mesh->indexBuffer) {
		renderSystem->DrawIndexed(DRAW_TRIANGLES, mesh->indexBuffer->Count());
	}
	else {
		renderSystem->Draw(DRAW_TRIANGLES, mesh->vertexBuffer->Count());
	}
}