#pragma once

#include "quest-game.h"
#include <Core/QGObject.h>

class QUEST_GAME Quest : public QGObject {
public:
	Quest() {
		id = 0;
		minLevel = 0;
	}
	~Quest() = default;

public:
	uint32_t id;
	std::string name;
	std::string description;

	std::vector<uint32_t> prerequisites;
	uint32_t minLevel;
};