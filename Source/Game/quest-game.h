
#pragma once

#ifdef QUEST_EXPORTS
#define QUEST_GAME __declspec(dllexport)
#else
#define QUEST_GAME __declspec(dllimport)
#endif