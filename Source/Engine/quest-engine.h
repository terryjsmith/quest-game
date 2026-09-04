
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
	#pragma warning(disable : 4244)
	#pragma warning(disable : 4251)

	#include <Windows.h>
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <typeindex>
#include <filesystem>

#include <assert.h>

#ifdef _WIN32
	#ifdef QUEST_ENGINE_EXPORTS
		#define QUEST_API __declspec(dllexport)
	#else
		#define QUEST_API __declspec(dllimport)
	#endif
#else
	#define QUEST_API
#endif

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef glm::vec2 vector2;
typedef glm::vec3 vector3;
typedef glm::vec4 vector4;
typedef glm::quat quaternion;
typedef glm::mat3 matrix3;
typedef glm::mat4 matrix4;

#include <nlohmann/json.hpp>

#define QGASSERT(test, msg) { assert(test); }