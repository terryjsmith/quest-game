
#ifndef qgworld_h
#define qgworld_h

#include <Core/QGEntity.h>

class QUEST_API QGWorld : public QGObject {
public:
	QGWorld() : m_nextID(0) { }
	~QGWorld() = default;

	static QGWorld* GetInstance();

	QGEntity* CreateEntity(std::string name = "", uint64_t id = 0) {
		QGEntity* entity = new QGEntity();
		entity->name = name;

		if (id == 0) id = ++m_nextID;
		entity->id = id;
		m_entities[id] = entity;
		return(entity);
	}

	void AddEntity(QGEntity* entity) {
		if (entity->id == 0) {
			entity->id = ++m_nextID;
		}

		m_entities[entity->id] = entity;
	}

	QGEntity* FindEntity(std::string name) {
		auto it = m_entities.begin();
		for (; it != m_entities.end(); it++) {
			if (it->second->name == name) return(it->second);
		}
		return(0);
	}

	QGEntity* FindEntity(uint64_t id) {
		auto it = m_entities.find(id);
		if (it != m_entities.end()) return(it->second);
		return(0);
	}

	void RemoveEntity(QGEntity* entity) {
		auto it = m_entities.begin();
		for (; it != m_entities.end(); it++) {
			if (it->second == entity) {
				m_entities.erase(it);
				return;
			}
		}
	}

	void RemoveEntity(uint64_t id) {
		m_entities.erase(id);
	}

	template<class T> std::vector<T*> GetComponents() {
		std::vector<T*> retval;

		auto it = m_entities.begin();
		for (; it != m_entities.end(); it++) {
			T* c = it->second->GetComponent<T>();
			if (c) {
				retval.push_back(c);
			}
		}

		return(retval);
	}

	std::map<uint64_t, QGEntity*> GetEntities() {
		return m_entities;
	}

protected:
	std::map<uint64_t, QGEntity*> m_entities;
	uint64_t m_nextID;

	static QGWorld* m_instance;
};

#endif