
#include <Render/QGRenderSystem.h>
#include <Core/QGWorld.h>

QGRenderSystem::QGRenderSystem() {
	m_scene = new QGScene();
}

void QGRenderSystem::Initialize(int width, int height) {
	for (auto it = m_passes.begin(); it != m_passes.end(); it++) {
		it->second->Initialize(width, height);
	}
}

void QGRenderSystem::Update(float delta) {
	QGWorld* world = QGWorld::GetInstance();
	m_scene->meshes = world->GetComponents<QGMeshComponent>();

	// Get the first camera component
	std::vector<QGCameraComponent*> cameras = world->GetComponents<QGCameraComponent>();
	if (cameras.size()) {
		m_scene->camera = cameras[0];
	}
}

void QGRenderSystem::Render() {
	// If we don't have a camera yet, nothing to render
	if (m_scene->camera == 0) {
		return;
	}

	// Iterate over render passes
	for (auto it = m_passes.begin(); it != m_passes.end(); it++) {
		it->second->Render(m_scene);
	}
}