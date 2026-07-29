
#ifndef qgapplication_h
#define qgapplication_h

#include <Core/QGSystem.h>

class QUEST_API QGApplication : public QGObject {
public:
	QGApplication() = default;
	~QGApplication() = default;

	static QGApplication* GetInstance();

	void Initialize() {
		auto it = m_systems.begin();
		for (; it != m_systems.end(); it++) {
			(*it)->system->Initialize();
		}
	}

	void Update(float delta);

	/**
	 * Create a new system and specify ticks per second (fixed or 0 (zero) for every frame)
	 */
	template<class T> T* CreateSystem(float tickRate = 0.0f) {
		T* system = new T();

		QGRegisteredSystem* rs = new QGRegisteredSystem();
		rs->system = system;
		rs->tickRate = (tickRate > 0.0f) ? 1.0f / tickRate : 0.0f;

		m_systems.push_back(rs);
		return(system);
	}

	template<class T> T* GetSystem() {
		auto it = m_systems.begin();
		for (; it != m_systems.end(); it++) {
			T* system = dynamic_cast<T*>((*it)->system);
			if (system) return(system);
		}
		return(0);
	}

	struct QGRegisteredSystem {
		QGSystem* system;
		float tickRate;
		float ticker;
	};

protected:
	static QGApplication* m_instance;
	std::vector<QGRegisteredSystem*> m_systems;
};

template<class T> T* GetQGSystem() {
	QGApplication* app = QGApplication::GetInstance();
	return(app->GetSystem<T>());
}

#endif