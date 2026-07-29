
#include <Core/QGApplication.h>

QGApplication* QGApplication::m_instance = 0;

QGApplication* QGApplication::GetInstance() {
	if (m_instance == 0) m_instance = new QGApplication();
	return(m_instance);
}

void QGApplication::Update(float delta) {
	// Loop over all systems
	auto it = m_systems.begin();
	for (; it != m_systems.end(); it++) {
		// If tickRate == 0, then update every frame
		if ((*it)->tickRate == 0) {
			(*it)->system->Update(delta);
			continue;
		}

		// Otherwise, increment the time since last update
		(*it)->ticker += delta;

		// If we're over our tick rate, then tick
		while ((*it)->ticker > (*it)->tickRate) {
			// Pass the specified tick rate as the delta
			(*it)->system->Update((*it)->tickRate);

			// Decrement the counter by the tick rate, loop again if needed
			(*it)->ticker -= (*it)->tickRate;
		}
	}
}