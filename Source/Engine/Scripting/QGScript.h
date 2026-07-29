
#ifndef qgscript_h
#define qgscript_h

#include <Core/QGObject.h>
#include <Scripting/QGMonoIncludes.h>

typedef void (*QGMonoScriptInitFunction)(MonoObject*, MonoException**);
typedef void (*QGMonoScriptUpdateFunction)(MonoObject*, float, MonoException**);
typedef void (*QGMonoScriptFixedUpdateFunction)(MonoObject*, float, MonoException**);
typedef void (*QGMonoScriptDestroyFunction)(MonoObject*, MonoException**);

class QUEST_API QGScript : public QGObject {
public:
	QGScript() = default;
	~QGScript() = default;

public:
	// Class name
	std::string className;

	// Variables in class (and type)
	std::map<std::string, uint32_t> vars;

	// Built-in functions
	QGMonoScriptInitFunction initFunc;
	QGMonoScriptUpdateFunction updateFunc;
	QGMonoScriptFixedUpdateFunction fixedUpdateFunc;
	QGMonoScriptDestroyFunction destroyFunc;
};

#endif