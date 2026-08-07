
#ifndef qggl_h
#define qggl_h

#ifdef _WIN32

#include <GL/gl3w.h>

#else

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#endif

#ifdef _DEBUG
#define GL_CHECK(stuff) stuff; { GLenum error = glGetError(); if(error) { printf("GL error %d\n", error); assert(error == 0); } }
#else
#define GL_CHECK(stuff) stuff;
#endif

void __stdcall OGLDebugMessage(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam);

#endif