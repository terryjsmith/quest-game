
#ifdef USE_OPENGL

#include <Render/GL/QGGLRenderSystem.h>
#include <Render/GL/QGGL.h>

void QGGLRenderSystem::InitializeGL() {
#ifdef _WIN32
	// Initialize OpenGL core profile (Windows only)
	if (gl3wInit()) {
		QGASSERT(false, "Failed to initialize OpenGL.");
	}
#endif

	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void QGGLRenderSystem::Clear(int mask) {
	GL_CHECK(glClear(mask));
}

void QGGLRenderSystem::SetClearColor(vector4 color) {
	GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
}

void QGGLRenderSystem::Draw(int type, int elements) {
	GL_CHECK(glDrawArrays(type, 0, elements));
}

void QGGLRenderSystem::DrawIndexed(int type, int elements) {
	GL_CHECK(glDrawElements(type, elements, GL_UNSIGNED_INT, 0));
}

void QGGLRenderSystem::SetViewport(int width, int height) {
	GL_CHECK(glViewport(0, 0, width, height));
}

void QGGLRenderSystem::EnableDepthTest(int type) {
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthFunc(type));
}

void QGGLRenderSystem::DisableDepthTest() {
	GL_CHECK(glDisable(GL_DEPTH_TEST));
}

#endif