#version 450

struct Character
{
	vec3 position;
	vec2 dimensions;
	uint layer;
};

layout(std430, binding = 0) restrict readonly buffer Characters
{
	Character characters[];
} b_characters;

layout(std140, binding = 1) uniform World
{
	mat4 view;
	mat4 projection;
} u_world;

layout(location = 0) out vec2 v_uvs;
layout(location = 1) flat out uint v_layer;

const vec2 c_quad_vertex[6] = vec2[](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),

	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0)
);

const vec2 c_quad_uvs[6] = vec2[](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),

	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0)
);

void main()
{
	uint currentQuad = gl_VertexIndex / 6;
	uint currentVertex = gl_VertexIndex % 6;

	Character currentCharacter = b_characters.characters[currentQuad];

	vec2 scaled = c_quad_vertex[currentVertex];
	vec3 worldPosition = currentCharacter.position + vec3(scaled, 0.0);

	gl_Position = u_world.projection * u_world.view * vec4(worldPosition, 1.0);
	v_layer = currentCharacter.layer;
	v_uvs = c_quad_uvs[currentVertex];
}