
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
    out_diffuse = vec4(texture(inputTexture, frag_texcoord.st).rgb, 1.0);
    //out_diffuse = vec4(0.0, 1.0, 1.0, 1.0);
}
