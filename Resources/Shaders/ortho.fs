#version 400 core

/**
 * In variables
 */
in vec2 frag_texcoord;

/**
 * Uniforms
 */

// Sample textures
uniform sampler2D inputTexture;

/**
 * Out variables
 */
out vec4 out_diffuse;

/**
 * Main
 */
void main () {
    out_diffuse = texture(inputTexture, frag_texcoord.st);
    //out_diffuse = vec4(0.0, 1.0, 1.0, 1.0);
}