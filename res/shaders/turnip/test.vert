#version 460

layout (location = 0) in vec3 a_position;
layout (location = 1) in float a_uvx;
layout (location = 2) in vec3 a_normals;
layout (location = 3) in float a_uvy;

struct Object
{
    mat4 model;
};

layout (std430, binding = 0) restrict readonly buffer WorldData
{
    mat4 view;
    mat4 projection;
} b_world;

layout (std430, binding = 1) restrict readonly buffer ObjectData
{
    Object data[];
} b_objects;

layout (location = 0) out vec2 v_uvs;
layout (location = 1) out vec3 v_normals;

void main()
{
    /* Draw index */
    Object object = b_objects.data[gl_InstanceIndex];

    gl_Position = b_world.projection * b_world.view * object.model * vec4(a_position, 1.0);  

    v_uvs = vec2(a_uvx, a_uvy);
    v_normals = a_normals;
}