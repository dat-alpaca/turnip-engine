#version 450
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uvs;

layout(binding = 0) uniform UBO
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat3 inverse_transpose;
	vec3 camera_position;
} ubo;

layout(location = 0) out vec3 v_world_position;
layout(location = 1) out vec2 v_uvs;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_camera_position;

void main()
{
	v_world_position = vec3(ubo.model * vec4(a_position, 1.0));
	v_uvs = a_uvs;
	v_normal = ubo.inverse_transpose * a_normal;
	v_camera_position = ubo.camera_position;

	gl_Position = ubo.projection * ubo.view * vec4(v_world_position, 1.0);
}