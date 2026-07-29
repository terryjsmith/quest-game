
#ifndef qgsystem_h
#define qgsystem_h

#include <Core/QGObject.h>

class QUEST_API QGSystem : public QGObject {
public:
	QGSystem() = default;
	virtual ~QGSystem() = default;

	virtual void Initialize() { }
	virtual void Update(float delta) { }
	virtual void Shutdown() { }
};

#endif