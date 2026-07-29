
#ifndef qginputsystem_h
#define qginputsystem_h

#include <IO/QGInputDevice.h>
#include <IO/QGInputEvent.h>
#include <Core/QGSystem.h>

class QUEST_API QGInputSystem : public QGSystem {
public:
	QGInputSystem() : m_nextCommandID(0) { }
	~QGInputSystem() = default;

	/**
	 * Initialize (and capture any device changes)
	 */
	void Initialize();

	/**
	 * Create or register a new input device
	 */
	template<class T> T* CreateInputDevice() {
		T* device = new T();
		m_devices.push_back(device);
		device->Initialize();
		return(device);
	}

	/**
	 * Find an existing input device
	 */
	template<class T> T* FindInputDevice() {
		auto it = m_devices.begin();
		for (; it != m_devices.end(); it++) {
			T* device = dynamic_cast<T*>(*it);
			if (device) return(device);
		}
		return(0);
	}

	/**
	 * Create a command
	 */
	void RegisterCommand(uint32_t id, std::string command) {
		m_commandIDs[command] = id;
		m_commandNames[id] = command;
		m_commandStates[command] = 0.0f;
	}

	/**
	 * Associate a command to be triggered by an input event
	 */
	void AssociateCommandInput(std::string command, QGInputDevice* device, int button, float multiplier = 1.0f) {
		QGRegisteredCommand* qic = new QGRegisteredCommand();
		qic->command = command;
		qic->device = device;
		qic->button = button;
		qic->multiplier = multiplier;

		m_commands.push_back(qic);
	}

	/**
	 * Get the mapping for a command input
	 */
	std::string GetCommandName(uint32_t id) {
		return(m_commandNames[id]);
	}

	/**
	 * Get the current state of a command
	 */
	float GetCommandState(std::string command) {
		return m_commandStates[command];
	}

	/**
	 * Callback function for input events (to trigger commands)
	 */
	static void InputEventCallbackHandler(QGEvent* event, QGObject* device);

protected:
	struct QGRegisteredCommand {
		std::string command;
		QGInputDevice* device;
		int button;
		float multiplier;
	};

	// Registered devices
	std::vector<QGInputDevice*> m_devices;

	// Registered commands
	std::vector<QGRegisteredCommand* > m_commands;
	std::map<std::string, uint32_t> m_commandIDs;
	std::map<uint32_t, std::string> m_commandNames;
	uint32_t m_nextCommandID;

	// Command states
	std::map<std::string, float> m_commandStates;
};

#endif