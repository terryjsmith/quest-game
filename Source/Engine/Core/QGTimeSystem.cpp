
#include <Core/QGTimeSystem.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <time.h>

void QGTimeSystem::Initialize() {
    Timestamp(&m_startupTime);
}

void QGTimeSystem::Timestamp(struct timespec* ts) {
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
#else
    timespec_get(ts, TIME_UTC);
#endif
}

timespec QGTimeSystem::Diff(timespec* start, timespec* end) {
    timespec t;

    if (end->tv_nsec - start->tv_nsec < 0) {
        t.tv_sec = end->tv_sec - start->tv_sec - 1;
        t.tv_nsec = end->tv_nsec - start->tv_nsec + 1000000000.0f;
    }
    else {
        t.tv_sec = end->tv_sec - start->tv_sec;
        t.tv_nsec = end->tv_nsec - start->tv_nsec;
    }
    return(t);
}

uint64_t QGTimeSystem::Tick() {
    if (m_overrideTick) return m_overrideTick;

    timespec t;
    Timestamp(&t);

    timespec diff = QGTimeSystem::Diff(&m_startupTime, &t);
    double d = (double)diff.tv_sec + ((double)diff.tv_nsec / 1000000000);

    return(std::floor(d * QG_TICKS_PER_SECOND));
}