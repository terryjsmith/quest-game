#version 400 core

/**
 * In variables
 */
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vertex_texcoord0;
layout(location = 5) in vec3 vertex_normal;

/**
 * Uniforms
 */

// Matrices
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

/**
 * Out variables
 */
out vec3 frag_position;
out vec3 frag_vcolor;
out vec2 frag_texcoord0;
out vec3 frag_normal;

/**
 * Main (default)
 */
void main () {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex_position.xyz, 1.0);
    
    // Send through position, normal and texture coords
    frag_position = mat3(viewMatrix * modelMatrix) * normalize(vertex_position.xyz);
    frag_vcolor = vertex_color;
    frag_texcoord0 = vertex_texcoord0;
    frag_normal = mat3(viewMatrix * modelMatrix) * normalize(vertex_normal.xyz);
}