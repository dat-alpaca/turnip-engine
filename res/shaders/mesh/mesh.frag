#version 450 core
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec3 v_world_position;
layout(location = 1) in vec2 v_uvs;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_camera_position;

layout(binding = 1) uniform sampler2D u_diffuse_texture;
layout(binding = 2) uniform sampler2D u_normal_texture;
layout(binding = 3) uniform sampler2D u_metallic_texture;
layout(binding = 4) uniform sampler2D u_roughness_texture;

vec3 t_light_pos = vec3(4, 6, 0.5);
vec3 t_light_color = vec3(1.0);

const float PI = 3.14159265359;

// Utils:
float distribution_ggx(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geometry_schlick_ggx(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_ggx(NdotV, roughness);
    float ggx1 = geometry_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
	// Normal:
	vec3 normal = texture(u_normal_texture, v_uvs).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	// Camera:
	vec3 camera_direction = normalize(v_camera_position - v_world_position);

	// Light Direction:
	
	
	// Textures:
	vec3 albedo = texture(u_diffuse_texture, v_uvs).rgb;
	float roughness = texture(u_roughness_texture, v_uvs).r;
	float metallic = texture(u_metallic_texture, v_uvs).r;

	vec3 Lo = vec3(0.0);
	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

	// Per light:
	vec3 light_direction = normalize(t_light_pos - v_world_position);
	vec3 half_direction = normalize(light_direction + v_world_position);
	float light_distance = length(t_light_pos - v_world_position);

	float attenuation = 1.0 / (light_distance * light_distance);
	vec3 radiance = t_light_color * attenuation;

	// Cook-Torrance:
	float ndf = distribution_ggx(normal, half_direction, roughness);
	float G   = geometry_smith(normal, camera_direction, light_direction, roughness);      
    vec3 F    = fresnel_schlick(clamp(dot(half_direction, camera_direction), 0.0, 1.0), F0);

	vec3 numerator    = ndf * G * F; 
    float denominator = 4.0 * max(dot(normal, camera_direction), 0.0) * max(dot(normal, light_direction), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;	  

	float NdotL = max(dot(normal, light_direction), 0.0);    
	Lo += (kD * albedo / PI + specular) * radiance * NdotL;

	vec3 ambient = vec3(0.03) * albedo;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    
	// gamma correct
	color = pow(color, vec3(1.0/2.2)); 

    out_color = vec4(color, 1.0);
}