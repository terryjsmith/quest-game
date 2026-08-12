
#ifndef qgmonocomponenttype_h
#define qgmonocomponenttype_h

#include <Core/QGObject.h>
#include <Scripting/QGMonoIncludes.h>
#include <Scripting/QGScriptObject.h>

typedef void (*QGMonoComponentSerializeFunction)(MonoObject*, MonoException**);
typedef void (*QGMonoComponentDeserializeFunction)(MonoObject*, MonoException**);

class QUEST_API QGMonoComponentType : public QGObject {
public:
	QGMonoComponentType() = default;
	~QGMonoComponentType() = default;

public:
	// Class name
	std::string className;

	// Variables in class (and type)
	std::map<std::string, uint32_t> vars;

	// Cached functions
	QGMonoComponentSerializeFunction serializeFunc;
	QGMonoComponentDeserializeFunction deserializeFunc;
};

#endif