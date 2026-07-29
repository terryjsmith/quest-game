
#ifndef qgentity_h
#define qgentity_h

#include <Core/QGComponent.h>
#include <Core/QGTransform.h>
#include <Core/QGSerializable.h>

class QUEST_API QGEntity : public QGObject, public QGSerializable {
public:
	QGEntity() : id(0) { }
	~QGEntity() {
		auto it = m_components.begin();
		for (; it != m_components.end(); it++) {
			delete (*it);
		}
	}

	template<class T> T* CreateComponent() {
		T* component = new T();
		QGComponent* c = dynamic_cast<QGComponent*>(component);
		c->m_parent = this;

		m_components.push_back(component);
		return(component);
	}

	void AddComponent(QGComponent* component) {
		component->m_parent = this;
		m_components.push_back(component);
	}

	template<class T> T* GetComponent() {
		for (auto it = m_components.begin(); it != m_components.end(); it++) {
			T* component = dynamic_cast<T*>(*it);
			if (component) return(component);
		}

		return(0);
	}

	void RemoveComponent(QGComponent* component) {
		auto it = m_components.begin();
		for (; it != m_components.end(); it++) {
			if ((*it) == component) {
				m_components.erase(it);
				return;
			}
		}
	}

	/**
	 * Get all components
	 */
	std::vector<QGComponent*> GetComponents() { 
		return m_components; 
	}

	/**
	 * Serialize
	 */
	void Serialize(QGDataRecord* record) {
		record->Set("name", name);
		record->Set("id", id);

		record->Set("position", transform.position);
		record->Set("scaling", transform.scaling);
		record->Set("rotation", transform.rotation);
	}

	void Deserialize(QGDataRecord* record) {
		this->name = record->Get("name").AsString();
		this->id = record->Get("id").AsUInt64();

		this->transform.position = record->Get("position").AsVector3();
		this->transform.scaling = record->Get("scaling").AsVector3();
		this->transform.rotation = record->Get("rotation").AsQuaternion();
	}

public:
	std::string name;
	uint64_t id;
	QGTransform transform;

protected:
	std::vector<QGComponent*> m_components;
};

#endif