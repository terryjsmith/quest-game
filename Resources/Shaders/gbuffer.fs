#version 400 core

/**
 * In variables
 */
in vec3 frag_position;
in vec3 frag_vcolor;
in vec2 frag_texcoord0;
in vec3 frag_normal;

/**
 * Uniforms
 */

// Sample textures
uniform sampler2D diffuseTexture;

/**
 * Out variables
 */
layout (location = 0) out vec3 out_position;
layout (location = 1) out vec4 out_diffuse;
layout (location = 2) out vec3 out_normal;

/**
 * Main
 */
void main () {
    out_position = frag_position;
    out_diffuse = vec4(frag_vcolor, 1.0);
    out_diffuse = texture(diffuseTexture, frag_texcoord0.st);
    out_normal = frag_normal;
}
