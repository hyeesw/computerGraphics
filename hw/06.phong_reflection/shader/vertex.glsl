#version 120                  // GLSL 1.20

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;

// Output variables to pass to the fragment shader
varying vec3 v_position;
varying vec3 v_normal;

// Uniform variables
uniform mat4 u_PVM;

// for phong shading
uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;

void main()
{
    // Transform the vertex position to world space
    v_position = (u_model_matrix * vec4(a_position, 1.0f)).xyz;
    
    // Transform the normal to world space
    v_normal   = normalize(u_normal_matrix * a_normal);

    // Transform the vertex position to clip space
    gl_Position = u_PVM * vec4(a_position, 1.0f);
}
