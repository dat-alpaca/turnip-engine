#version 450
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uvs;

layout(binding = 0) uniform UBO
{
	mat4 mvp;
} ubo;

layout(location = 0) out vec2 v_uvs;
layout(location = 1) out vec3 v_normal;

void main()
{
	v_uvs = a_uvs;
	v_normal = a_normal;
	gl_Position = ubo.mvp * vec4(a_position, 1.0);
}