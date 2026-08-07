#version 400 core

/**
 * In variables
 */
in vec3 frag_vcolor;
in vec2 frag_texcoord0;

/**
 * Uniforms
 */

// Sample textures
uniform sampler2D diffuseTexture;

/**
 * Out variables
 */
layout (location = 0) out vec4 out_diffuse;

/**
 * Main
 */
void main () {
    out_diffuse = vec4(frag_vcolor, 1.0);
    out_diffuse = texture(diffuseTexture, frag_texcoord0.st);
}
