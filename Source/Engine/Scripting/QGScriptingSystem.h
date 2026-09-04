
#ifndef qgscriptingsystem_h
#define qgscriptingsystem_h

#include <Core/QGSystem.h>

class QUEST_API QGScriptingSystem : public QGSystem {
public:
	QGScriptingSystem() = default;
	~QGScriptingSystem() = default;

	/**
	 * Start mono, load default engine library
	 */
	void Initialize();

	/**
	 * Update, calls update for all objects
	 */
	void Update(float delta);

	/**
	 * Load a script library
	 */
	void LoadScriptLibrary(std::string filename);
};

#endif