
#pragma once

#ifdef _WIN32
	#ifdef QUEST_EXPORTS
		#define QUEST_GAME __declspec(dllexport)
	#else
		#define QUEST_GAME __declspec(dllimport)
	#endif
#else
	#define QUEST_GAME
#endif

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif