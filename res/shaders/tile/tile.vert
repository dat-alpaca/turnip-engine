#version 450

struct Tile
{
	uint position[2];
	uint layer;
	uint flags;
};

layout(std430, binding = 0) restrict readonly buffer Tiles
{
	Tile tiles[];
} b_tiles;

layout(std140, binding = 1) uniform World
{
	mat4 viewProjection;
} u_world;

layout(location = 0) flat out uint v_layer;

const vec2 c_quad_vertex[6] = vec2[](
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

	Tile currentTile = b_tiles.tiles[currentQuad];
	vec2 tilePosition = vec2(currentTile.position[0], currentTile.position[1]);

	vec2 worldPosition = tilePosition + c_quad_vertex[currentVertex];

	gl_Position = u_world.viewProjection * vec4(worldPosition, 0.0, 1.0);
	v_layer = currentTile.layer;
}