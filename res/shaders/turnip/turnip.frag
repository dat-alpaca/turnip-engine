#version 460
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_nonuniform_qualifier : enable

#ifdef GL_EXT_nonuniform_qualifier
	#define NonUniformIndex(x) nonuniformEXT(x)
#else
	#define NonUniformIndex(x) (x)
#endif

layout (location = 0) out vec4 out_color;

layout (location = 0) in vec3 v_normal;
layout (location = 1) in vec2 v_uvs;
layout (location = 2) flat in uint v_textureID;

layout (binding = 3) uniform sampler2D u_textures[];

void main()
{
	vec3 normal = v_normal;
	out_color = texture(u_textures[NonUniformIndex(v_textureID)], v_uvs);
}