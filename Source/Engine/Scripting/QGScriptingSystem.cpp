
#include <Scripting/QGScriptingSystem.h>
#include <Scripting/QGScriptComponent.h>
#include <Core/QGWorld.h>

typedef void(__cdecl* QGScriptLibraryInitFunc)(void);

#ifndef _WIN32
	#define Sleep sleep
	#include <dlfcn.h>
#endif

void QGScriptingSystem::Initialize() {
	
}

void QGScriptingSystem::Update(float delta) {
	std::vector<QGScriptComponent*> components = QGWorld::GetInstance()->GetComponents<QGScriptComponent>();
	for (auto it = components.begin(); it != components.end(); it++) {
		(*it)->Update(delta);
	}
}

void QGScriptingSystem::LoadScriptLibrary(std::string filename) {
	// All platform specific code
#ifdef _WIN32
	std::string fullname = filename + ".dll";
	HINSTANCE hDll = LoadLibrary(TEXT(fullname.c_str()));
	QGScriptLibraryInitFunc initFunc = (QGScriptLibraryInitFunc)GetProcAddress(hDll, "qg_init_library");
	initFunc();
#else
	std::string fullname = filename + ".so";
	void* handle = dlopen(fullname.c_str(), RTLD_NOW);
	QGScriptLibraryInitFunc initFunc = (QGScriptLibraryInitFunc)dlsym(handle, "qg_init_library");
	initFunc();
#endif
}