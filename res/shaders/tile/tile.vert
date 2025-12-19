#version 450

struct Tile
{
	uint packedData;
};

vec2 get_tile_position(Tile tile)
{
	uint tileX     =  tile.packedData        & 0xFFu;
	uint tileY     = (tile.packedData >>  8) & 0xFFu;
	return vec2(tileX, tileY);	
}

uint get_tile_layer(Tile tile)
{
	return (tile.packedData >> 16) & 0xFFu;
}

uint get_tile_flags(Tile tile)
{
	return (tile.packedData >> 24) & 0xFFu;
}

layout(std430, binding = 0) restrict readonly buffer Tiles
{
	Tile tiles[];
} b_tiles;

layout(std140, binding = 1) uniform World
{
	mat4 viewProjection;
} u_world;

layout(location = 0) out vec2 v_uvs;
layout(location = 1) flat out uint v_layer;
layout(location = 2) flat out uint v_flags;

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

	Tile currentTile = b_tiles.tiles[currentQuad];
	vec2 tilePosition = get_tile_position(currentTile);

	vec2 worldPosition = tilePosition + c_quad_vertex[currentVertex];

	gl_Position = u_world.viewProjection * vec4(worldPosition, 0.0, 1.0);
	v_layer = get_tile_layer(currentTile);
	v_flags = get_tile_flags(currentTile);
	v_uvs = c_quad_uvs[currentVertex];
}