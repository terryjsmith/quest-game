#version 400 core

/**
 * In variables
 */
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vertex_texcoord0;
layout(location = 3) in vec4 vertex_boneids;
layout(location = 4) in vec4 vertex_boneweights;

/**
 * Uniforms
 */

// Animation
uniform mat4 finalBoneMatrices[100];

// Matrices
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

/**
 * Out variables
 */
out vec3 frag_vcolor;
out vec2 frag_texcoord0;

/**
 * Main (default)
 */
void main () {
	// Accumulate bone transformations based on weights
    mat4 boneTransform  = finalBoneMatrices[int(vertex_boneids.x)] * vertex_boneweights.x;
    boneTransform      += finalBoneMatrices[int(vertex_boneids.y)] * vertex_boneweights.y;
    boneTransform      += finalBoneMatrices[int(vertex_boneids.z)] * vertex_boneweights.z;
    boneTransform      += finalBoneMatrices[int(vertex_boneids.w)] * vertex_boneweights.w;

    // Transform position and normal
	// vec4 localPosition = vec4(vertex_position.xyz, 1.0);
    vec4 localPosition = boneTransform * vec4(vertex_position.xyz, 1.0);
    // vec4 localNormal   = boneTransform * vec4(normal, 0.0);
	
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * localPosition;
    
    // Send through position, normal and texture coords
    frag_vcolor = vertex_color;
    frag_texcoord0 = vertex_texcoord0;
}
