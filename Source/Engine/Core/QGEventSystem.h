
#ifndef qgeventsystem_h
#define qgeventsystem_h

#include <Core/QGSystem.h>
#include <Core/QGEvent.h>

/**
 * Event handling callback
 */
typedef void(*QGEventHandlingCallback)(QGEvent* ev, QGObject* obj);

class QUEST_API QGEventSystem : public QGSystem {
public:
	QGEventSystem() = default;
	~QGEventSystem() = default;

	// A subscriber to a specific event type or events on an object (or both)
	class QGEventHandler {
	public:
		QGEventHandler() : type(typeid(this)), ptr(0), callback(0) {}
		~QGEventHandler() = default;

	public:
		std::type_index type;
		QGObject* ptr;
		QGEventHandlingCallback callback;
	};

	template<class T> void Subscribe(QGEventHandlingCallback func, QGObject* obj = 0) {
		QGEventHandler* handler = new QGEventHandler();
		handler->callback = func;
		handler->ptr = obj;
		handler->type = typeid(T);
		m_handlers.push_back(handler);
	}

	template<class T>
	void Publish(T* event, QGObject* ptr = 0) {
		std::type_index ix = typeid(*event);

		auto it = m_handlers.begin();
		for (; it != m_handlers.end(); it++) {
			if (ix == (*it)->type) {
				if ((*it)->ptr == 0 || (*it)->ptr == ptr) {
					(*it)->callback(event, ptr);
				}
			}
		}
	}

protected:
	std::vector<QGEventHandler*> m_handlers;
};

#endif