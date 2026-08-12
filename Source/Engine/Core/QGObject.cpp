
#include <Core/QGObject.h>
#include <Core/QGMetaSystem.h>
#include <Core/QGApplication.h>

QGObjectType* QGObject::Type() {
	if (m_type == 0) {
		QGMetaSystem* metaSystem = GetQGSystem<QGMetaSystem>();
		m_type = metaSystem->GetType(this);
	}

	return(m_type);
}