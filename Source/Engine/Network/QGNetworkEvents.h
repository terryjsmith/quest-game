
#ifndef qgnetworkevents_h
#define qgnetworkevents_h

#include <Core/QGEvent.h>
#include <Core/QGEntity.h>

class QGNetworkClientConnectEvent : public QGEvent {
public:
	QGNetworkClientConnectEvent(uint32_t client_index, uint64_t client_id) : clientIndex(client_index), clientID(client_id) {}
	~QGNetworkClientConnectEvent() = default;

public:
	uint32_t clientIndex;
	uint64_t clientID;
};

class QGNetworkClientDisconnectEvent : public QGEvent {
public:
	QGNetworkClientDisconnectEvent(uint32_t client_index, uint64_t client_id) : clientIndex(client_index), clientID(client_id) {}
	~QGNetworkClientDisconnectEvent() = default;

public:
	uint32_t clientIndex;
	uint64_t clientID;
};

class QGPlayerConnectedEvent : public QGEvent {
public:
	QGPlayerConnectedEvent(uint64_t client_id, QGEntity* ent) : clientID(client_id), entity(ent) {}
	~QGPlayerConnectedEvent() = default;

public:
	uint64_t clientID;
	QGEntity* entity;
};

#endif