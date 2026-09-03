# set the minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.10)

# define the project name and metadata
project(quest-game VERSION 1.0)

set (CMAKE_CXX_STANDARD 20)

# set the output directory without the /Debug/ and /Release/ equivalents
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Must be set before defining targets
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# add libraries
set (ENGINE_LIBS "")
set (ENGINE_INCLUDE_DIRS "")

# add platform specific libraries
IF(WIN32)
	list (APPEND ENGINE_LIBS "legacy_stdio_definitions.lib")
	list (APPEND ENGINE_LIBS "OpenGL32.lib")
	list (APPEND ENGINE_LIBS "WS2_32.lib")
	list (APPEND ENGINE_LIBS "WinMM.Lib")
	list (APPEND ENGINE_LIBS "AdvAPI32.Lib")
	list (APPEND ENGINE_LIBS "Crypt32.Lib")
	list (APPEND ENGINE_LIBS "Dbghelp.lib")
	list (APPEND ENGINE_LIBS "ShLwApi.Lib")
	list (APPEND ENGINE_LIBS "Wldap32.Lib")
	list (APPEND ENGINE_LIBS "Version.lib")
	list (APPEND ENGINE_LIBS "Bcrypt.lib")
	# list (APPEND ENGINE_LIBS ${LIBRARY_ZLIB})
	# find_library(LIBRARY_ZLIB zlib "${DEPENDENCY_PATH}/zip/lib" NO_DEFAULT_PATH)
ENDIF()

IF(APPLE)
	list (APPEND ENGINE_LIBS "-framework IOKit")
	list (APPEND ENGINE_LIBS "-framework Cocoa")
	list (APPEND ENGINE_LIBS "-framework CoreVideo")
	list (APPEND ENGINE_LIBS "-framework CoreAudio")
	list (APPEND ENGINE_LIBS "-framework OpenGL")
	list (APPEND ENGINE_LIBS "-framework Security")
	list (APPEND ENGINE_LIBS "z")
	list (APPEND ENGINE_LIBS "iconv")
ENDIF()

# dependencies
add_subdirectory(Source/ThirdParty/assimp-6.0.5)
list (APPEND ENGINE_LIBS assimp::assimp)
list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/assimp-6.0.5/include")
list (APPEND ENGINE_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/Source/ThirdParty/assimp-6.0.5/include")

add_subdirectory(Source/ThirdParty/bullet3-3.25)
list (APPEND ENGINE_LIBS BulletCollision BulletDynamics LinearMath)
list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/bullet3-3.25/src")

add_subdirectory(Source/ThirdParty/gl3w)
list (APPEND ENGINE_LIBS gl3w)
list (APPEND ENGINE_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/Source/ThirdParty/gl3w/include")

add_subdirectory(Source/ThirdParty/glfw-3.4)
list (APPEND ENGINE_LIBS glfw)
list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/glfw-3.4/include")

add_subdirectory(Source/ThirdParty/json-3.12.0)
list (APPEND ENGINE_LIBS ${NLOHMANN_JSON_TARGET_NAME})
list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/json-3.12.0/include")

list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/glm/include")

add_subdirectory(Source/ThirdParty/netcode-1.3.3)
list (APPEND ENGINE_LIBS netcode)
list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/netcode-1.3.3")

list (APPEND ENGINE_INCLUDE_DIRS "Source/ThirdParty/stb")

# main library
set (ENGINE_SOURCE_FILES "")

# helper macros to add directories of files and clear list
macro (list_source_files dir)
	file (GLOB CPP_FILES "${dir}/*.cpp")
	file (GLOB H_FILES "${dir}/*.h")
	
	list (APPEND ENGINE_SOURCE_FILES ${CPP_FILES})
	list (APPEND ENGINE_SOURCE_FILES ${H_FILES})
endmacro()

macro (clear_source_files)
	set (ENGINE_SOURCE_FILES "")
endmacro()

# one more macro to group files into folders
macro (group_source_files folder dir)
	file (GLOB CPP_FILES "${dir}/*.cpp")
	file (GLOB H_FILES "${dir}/*.h")
	
	source_group("${folder}" FILES ${CPP_FILES} ${H_FILES})
endmacro()

list_source_files("Source/Engine/Core")
group_source_files("Source/Core" "Source/Engine/Core")

list_source_files("Source/Engine/IO")
group_source_files("Source/IO" "Source/Engine/IO")

list_source_files("Source/Engine/Network")
group_source_files("Source/Network" "Source/Engine/Network")

# list_source_files("Source/Engine/Audio")
# group_source_files("Source/Audio" "Source/Engine/Audio")

list_source_files("Source/Engine/Physics")
group_source_files("Source/Physics" "Source/Engine/Physics")

list_source_files("Source/Engine/Render")
list_source_files("Source/Engine/Render/GL")
list_source_files("Source/Engine/Render/Passes")
group_source_files("Source/Render" "Source/Engine/Render")
group_source_files("Source/Render/GL" "Source/Engine/Render/GL")
group_source_files("Source/Render/Passes" "Source/Engine/Render/Passes")

list_source_files("Source/Engine/Scripting")
group_source_files("Source/Scripting" "Source/Engine/Scripting")

add_library(quest-engine SHARED ${ENGINE_SOURCE_FILES})
target_link_libraries(quest-engine ${ENGINE_LIBS})

set_property(TARGET quest-engine APPEND PROPERTY COMPILE_DEFINITIONS "USE_OPENGL")
set_property(TARGET quest-engine APPEND PROPERTY COMPILE_DEFINITIONS "QUEST_ENGINE_EXPORTS")
set_property(TARGET quest-engine APPEND PROPERTY COMPILE_DEFINITIONS "GLFW_INCLUDE_GLCOREARB")
target_include_directories(quest-engine PRIVATE "Source/Engine" ${ENGINE_INCLUDE_DIRS})

clear_source_files()

##
## game client
##

add_executable(quest-cli Source/Client/quest-game.cpp)

set_property(TARGET quest-cli APPEND PROPERTY COMPILE_DEFINITIONS)
set_property(TARGET quest-cli APPEND PROPERTY COMPILE_DEFINITIONS "CURL_STATICLIB")
set_property(TARGET quest-cli APPEND PROPERTY COMPILE_DEFINITIONS "USE_OPENGL")
set_property(TARGET quest-cli APPEND PROPERTY COMPILE_DEFINITIONS "GLFW_INCLUDE_GLCOREARB")

target_link_libraries(quest-cli ${ENGINE_LIBS} quest-engine)
target_include_directories(quest-cli PRIVATE "Source/Engine" ${ENGINE_INCLUDE_DIRS})
add_dependencies(quest-cli quest-engine)

clear_source_files()

##
## game server
##

add_executable(quest-server Source/Server/quest-server.cpp)

set_property(TARGET quest-server APPEND PROPERTY COMPILE_DEFINITIONS)
set_property(TARGET quest-server APPEND PROPERTY COMPILE_DEFINITIONS "CURL_STATICLIB")

target_link_libraries(quest-server ${ENGINE_LIBS} quest-engine)
target_include_directories(quest-server PRIVATE "Source/Engine" ${ENGINE_INCLUDE_DIRS})
add_dependencies(quest-server quest-engine)

clear_source_files()

##
## game DLL
##

list_source_files("Source/Game")
add_library(quest-game SHARED ${ENGINE_SOURCE_FILES})

set_property(TARGET quest-game APPEND PROPERTY COMPILE_DEFINITIONS)
set_property(TARGET quest-game APPEND PROPERTY COMPILE_DEFINITIONS "QUEST_EXPORTS")
set_property(TARGET quest-game APPEND PROPERTY COMPILE_DEFINITIONS "CURL_STATICLIB")

target_link_libraries(quest-game ${ENGINE_LIBS} quest-engine)
target_include_directories(quest-game PRIVATE "Source/Engine" "Source/Game" ${ENGINE_INCLUDE_DIRS})
add_dependencies(quest-game quest-engine)

clear_source_files()