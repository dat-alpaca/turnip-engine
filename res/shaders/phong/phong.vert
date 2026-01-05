#version 450
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uvs;

layout(binding = 0) uniform UBO
{
	mat4 mvp;
} ubo;

layout(location = 0) out vec2 v_uvs;

void main()
{
	v_uvs = a_uvs;
	gl_Position = ubo.mvp * vec4(a_position, 1.0);
}