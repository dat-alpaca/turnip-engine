#version 450

// Constants:
struct Tile { vec2 pos; uint layer; uint _pad; };

const vec2 c_quad_vertices[6] = vec2[6]
(
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0)
);

// Attributes & Buffers:
layout(location = 0) in uint a_tile_size;

layout(std430, binding = 0) readonly buffer VertexBuffer 
{
	Tile tiles[];
} b_vertex_buffer;

layout(binding = 1) uniform Camera 
{
	mat4 projview;
} u_camera;

// Varyings:
layout(location = 0) out vec2 v_uvs;
layout(location = 1) flat out uint v_layer;

void main()
{
	const uint vertex_int   	 = uint(gl_VertexID);
	const uint vertex_index 	 = vertex_int / 6u;
	const uint current_vertex    = vertex_int % 6u;

	Tile tile = b_vertex_buffer.tiles[vertex_index];
	
	vec2 local_position = c_quad_vertices[current_vertex] * a_tile_size;
    vec2 world = tile.pos + local_position;

	gl_Position = u_camera.projview * vec4(world, 0.0, 1.0);
	v_uv = c_quad_vertices[current_vertex];
    v_layer = tile.layer;
}