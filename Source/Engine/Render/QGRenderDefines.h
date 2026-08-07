
#ifndef qgrenderdefines_h
#define qgrenderdefines_h

#include <quest-engine.h>

// Color types
extern int COLOR_RED;
extern int COLOR_RED16F;
extern int COLOR_RGB;
extern int COLOR_RGBA;
extern int COLOR_RGB16F;
extern int COLOR_RED32F;
extern int COLOR_DEPTH_COMPONENT24;
extern int COLOR_DEPTH_COMPONENT32F;
extern int COLOR_DEPTH_COMPONENT;

// Value tests
extern int TEST_LESS;
extern int TEST_EQUAL;
extern int TEST_GREATER;
extern int TEST_LEQUAL;
extern int TEST_GEQUAL;

// Filters
extern int TEXTURE_FILTER_NEAREST;
extern int TEXTURE_FILTER_LINEAR;
extern int TEXTURE_FILTER_MIPMAP_LINEAR;

extern int TEXTURE_FILTER_MINIFICATION;
extern int TEXTURE_FILTER_MAGNIFICATION;

// Clear bits
extern int COLOR_BUFFER_BIT;
extern int DEPTH_BUFFER_BIT;
extern int STENCIL_BUFFER_BIT;

// Drawing types
extern int DRAW_TRIANGLES;
extern int DRAW_TRIANGLE_STRIP;
extern int DRAW_PATCHES;

// Fill modes
extern int POLYMODE_FILL;
extern int POLYMODE_LINE;

// Rendering destinations
extern int DRAW_BUFFER_NONE;
extern int DRAW_BUFFER_FRONT;
extern int DRAW_BUFFER_BACK;

// Texture wrap modes
extern int TEXTURE_WRAP_NONE;
extern int TEXTURE_WRAP_REPEAT;

// Blend types
extern int BLEND_ZERO;
extern int BLEND_ONE;
extern int BLEND_SRC_COLOR;
extern int BLEND_ONE_MINUS_SRC_COLOR;
extern int BLEND_SRC_ALPHA;
extern int BLEND_ONE_MINUS_SRC_ALPHA;
extern int BLEND_DST_ALPHA;
extern int BLEND_ONE_MINUS_DST_ALPHA;

extern int TEXTURE_SLOT_POSITIVE_X;
extern int TEXTURE_SLOT_NEGATIVE_X;
extern int TEXTURE_SLOT_POSITIVE_Y;
extern int TEXTURE_SLOT_NEGATIVE_Y;
extern int TEXTURE_SLOT_POSITIVE_Z;
extern int TEXTURE_SLOT_NEGATIVE_Z;

#endif