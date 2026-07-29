
#ifndef qgscriptobject_h
#define qgscriptobject_h

#include <Core/QGObject.h>
#include <Scripting/QGMonoIncludes.h>

class QUEST_API QGScriptObject : public QGObject {
public:
	QGScriptObject() = default;
	~QGScriptObject() = default;

public:
	QGObject* localObj;
	MonoObject* remoteObj;
};

#endif