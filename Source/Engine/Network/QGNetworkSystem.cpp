
#include <Network/QGNetworkSystem.h>

void QGNetworkSystem::Initialize() {
    if (netcode_init() != NETCODE_OK) {
        QGASSERT(false, "Unable to initialize networking libbrary.");
    }

    netcode_log_level(NETCODE_LOG_LEVEL_INFO);
}

void QGNetworkSystem::RegisterPacketCallback(uint8_t type, QGPacketHandlingCallback func) {
    auto it = m_handlers.find(type);
    if (it == m_handlers.end()) {
        m_handlers[type] = std::vector<QGPacketHandlingCallback>();
    }

    m_handlers[type].push_back(func);
}

void QGNetworkSystem::ProcessPacket(QGNetworkPacket* packet) {
    // Process / send messages
    auto it = m_handlers.find(packet->env.type);
    if (it != m_handlers.end()) {
        // Loop through handlers
        for (auto iit = m_handlers[packet->env.type].begin(); iit != m_handlers[packet->env.type].end(); iit++) {
            (*iit)(packet);
        }
    }
}