
#include <Scripting/QGScriptingSystem.h>
#include <Scripting/QGScriptComponent.h>
#include <Core/QGWorld.h>

typedef void(*QGScriptLibraryInitFunc)(void);

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
	std::string fullname = filename + "d.dll";
	HINSTANCE hDll = LoadLibrary(TEXT(fullname.c_str()));
	QGScriptLibraryInitFunc initFunc = (QGScriptLibraryInitFunc)GetProcAddress(hDll, "qg_init_library");
	initFunc();
#else
	std::string fullname = "./lib" + filename + ".so";
	void* handle = dlopen(fullname.c_str(), RTLD_LAZY);
	if (handle == 0) {
		QGASSERT(false, "Unable to load library: %s", fullname.c_str());
		return;
	}

	dlerror();

	QGScriptLibraryInitFunc initFunc = (QGScriptLibraryInitFunc)dlsym(handle, "qg_init_library");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		QGASSERT(false, "Unable to load script function.");
		dlclose(handle);
	}

	initFunc();
#endif
}