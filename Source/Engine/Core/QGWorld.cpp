
#include <Core/QGWorld.h>

QGWorld* QGWorld::m_instance = 0;

QGWorld* QGWorld::GetInstance() {
	if (m_instance == 0) m_instance = new QGWorld();
	return(m_instance);
}