
#ifndef qginputcommand_h
#define qginputcommand_h

#include <Core/QGEvent.h>

class QUEST_API QGInputCommand : public QGEvent {
public:
	QGInputCommand(std::string command, uint32_t type, float state) {
		this->command = command;
		this->type = type;
		this->state = state;
	}
	~QGInputCommand() = default;

public:
	std::string command;
	uint32_t type;
	float state;
};

#endif