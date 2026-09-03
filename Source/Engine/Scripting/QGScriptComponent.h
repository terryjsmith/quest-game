
#ifndef qgscriptcomponent_h
#define qgscriptcomponent_h

#include <Core/QGComponent.h>
#include <Core/QGVariant.h>

class QUEST_API QGScriptComponent : public QGComponent {
public:
	QGScriptComponent() = default;
	virtual ~QGScriptComponent() = default;

	/**
	* Overridable functions
	*/
	virtual void Initialize();
	virtual void Update(float delta);
	virtual void FixedUpdate(float delta);
	virtual void Destroy();
};

#endif