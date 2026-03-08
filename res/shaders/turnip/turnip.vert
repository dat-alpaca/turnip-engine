#version 460

struct Vertex
{
    vec3 a_position;
    float a_uvx;
    vec3 a_normal;
    float a_uvy;
};

struct Mesh
{
    mat4 model;
    mat4 inverseTranspose;
};

layout (std430, binding = 0) restrict readonly buffer Vertices
{
    Vertex vertices[];
} b_vertices;

layout (std430, binding = 1) restrict readonly buffer Meshes
{
    Mesh meshes[];
} b_meshes;

layout (std430, binding = 2) restrict readonly buffer World
{
    mat4 viewProjection;
} b_world;

layout (location = 0) out vec3 v_normal;
layout (location = 1) out vec2 v_uvs;

void main()
{
    Mesh mesh = b_meshes.meshes[gl_DrawID];
    Vertex vertex = b_vertices.vertices[gl_VertexIndex];

    gl_Position = b_world.viewProjection * mesh.model * vec4(vertex.a_position, 1.0);
    
    v_normal = (mesh.inverseTranspose * vec4(vertex.a_normal, 0.0)).xyz;
    v_uvs = vec2(vertex.a_uvx, vertex.a_uvy);
}