#version 450
layout(location = 0) in vec3 a_position;

layout(binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
}
ubo;

void main()
{
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_position, 1.0);
}