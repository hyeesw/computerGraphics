#version 120                  // GLSL 1.20

// Input variables from the vertex shader
varying vec3 v_position;
varying vec3 v_normal;

// Uniform variables
uniform mat4 u_PVM;

// for phong shading
uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;

uniform vec3 u_light_position;
uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;

uniform vec3  u_obj_ambient;
uniform vec3  u_obj_diffuse;
uniform vec3  u_obj_specular;
uniform float u_obj_shininess;

uniform vec3 u_camera_position;
// uniform mat4 u_view_matrix;


void main()
{
    // Normalized normal vector in world space
    vec3 normal = normalize(v_normal);

    // Light direction
    vec3 light_direction = normalize(u_light_position);

    // View direction
    vec3 view_direction = normalize(u_camera_position - v_position);

    // Reflection direction for specular calculation
    vec3 reflection_direction = reflect(-light_direction, normal);

    // Ambient component
    vec3 ambient = u_light_ambient * u_obj_ambient;

    // Diffuse component (Lambertian reflectance)
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = u_light_diffuse * (diff * u_obj_diffuse);

    // Specular component (Phong model)
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), u_obj_shininess);
    vec3 specular = u_light_specular * (spec * u_obj_specular);

    // Combine all components
    vec3 color = ambient + diffuse + specular;

    // Output the final color
    gl_FragColor = vec4(color, 1.0f);
}
