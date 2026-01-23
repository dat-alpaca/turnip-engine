#version 450
layout(location = 0) in vec3 a_position;

layout(binding = 0) uniform UBO
{
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) out vec3 v_uvs;

void main()
{
	v_uvs = a_position;
	vec4 position = ubo.proj * ubo.view * vec4(a_position, 1.0);
	gl_Position = position.xyww;
}