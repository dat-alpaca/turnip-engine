#version 450 core
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 v_uvs;
layout(location = 1) flat in uint v_layer;
layout(location = 2) flat in uint v_flags;

layout(binding = 2) uniform sampler2DArray u_texture;

void decode_flags(uint flags, out uint animation_size, out uint flip, out uint enable)
{
    flags &= 0xFFu;

    animation_size = flags & 0xFCu;
    flip   = (flags >> 1u) & 0x01u;
    enable = (flags >> 0u) & 0x01u;
}

void main() 
{
    uint flags = v_flags, animation_size, flip, enable;
    decode_flags(flags, animation_size, flip, enable);

    vec2 uvs = v_uvs;
    if(enable == 0u && flip == 0u)
        discard;

    if(flip == 1 && enable == 1)
        uvs.x = 1.0 - v_uvs.x;
    
    if(flip == 1 && enable == 0)
        uvs.y = 1.0 - v_uvs.y;

    out_color = texture(u_texture, vec3(uvs.xy, float(v_layer)));
}