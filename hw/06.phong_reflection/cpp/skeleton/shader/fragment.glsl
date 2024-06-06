// FIXED : 조명 계산이 fragment shader 단계에서 진행되도록 수정
#version 120                  // GLSL 1.20

varying vec3 v_position_wc;
varying vec3 v_normal_wc;

uniform vec3 u_light_position;
uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;

uniform vec3  u_obj_ambient;
uniform vec3  u_obj_diffuse;
uniform vec3  u_obj_specular;
uniform float u_obj_shininess;

uniform vec3 u_camera_position;

void main()
{
  vec3 color = vec3(0.0);

  vec3 normal_wc = normalize(v_normal_wc);
  vec3 light_dir = normalize(u_light_position - v_position_wc);
  vec3 view_dir  = normalize(u_camera_position - v_position_wc);

  // ambient
  color += (u_light_ambient * u_obj_ambient);
  
  // diffuse 
  float ndotl = max(dot(normal_wc, light_dir), 0.0);
  color += (ndotl * u_light_diffuse * u_obj_diffuse);

  // specular
  vec3 reflect_dir = reflect(-light_dir, normal_wc); 
  float rdotv = max(dot(view_dir, reflect_dir), 0.0);
  color += (pow(rdotv, u_obj_shininess) * u_light_specular * u_obj_specular);

  gl_FragColor = vec4(color, 1.0f);
}