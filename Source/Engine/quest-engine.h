
#define WIN32_LEAN_AND_MEAN
#define QUEST_API
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <typeindex>
#include <filesystem>

#include <assert.h>

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

#define QGASSERT(test, msg) { assert(test); }