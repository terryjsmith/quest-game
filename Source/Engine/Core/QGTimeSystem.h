
#ifndef qgtimesystem_h
#define qgtimesystem_h

#include <Core/QGSystem.h>

#define QG_TICKS_PER_SECOND	60

class QUEST_API QGTimeSystem : public QGSystem {
public:
	QGTimeSystem() : m_overrideTick(0) {}
	~QGTimeSystem() = default;

	void Initialize();

	/**
	 * Get current high precision timestamp
	 */
	void Timestamp(struct timespec* ts);

	/**
	 * Get difference between two timespecs
	 */
	static timespec Diff(timespec* start, timespec* end);

	/**
	 * Get startup time
	 */
	timespec StartupTime() { return m_startupTime; }
	void StartupTime(timespec t) { m_startupTime = t; }

	/**
	 * Get current tick
	 */
	uint64_t Tick();
	void OverrideTick(uint64_t tick) {
		m_overrideTick = tick;
	}

protected:
	timespec m_startupTime;
	uint64_t m_overrideTick;
};

#endif