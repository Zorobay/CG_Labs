#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 binormal;

uniform mat4 ModelViewProj; // Model -> Clip space
uniform mat4 World; // Model -> World space
uniform mat4 WorldIT; // Inverse transpose
uniform vec3 LightPos; // Defined in world space
uniform vec3 CamPos; // Defined in world space

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 view;
    vec3 light;
	vec2 texcoord;
	mat3 TBN;
} vs_out;


void main()
{

    // Construct TBN matrix for normal mapping
    vec3 T = normalize(vec3(World * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(World * vec4(binormal, 0.0)));
    vec3 N = normalize(vec3(World * vec4(normal, 0.0)));
    mat3 TBN = mat3(T, B, N);

    // Calculate light and view stuffs
    vec3 worldPos = (World * vec4(vertex, 1)).xyz;
    vs_out.view = CamPos - worldPos;
    vs_out.light = LightPos - worldPos;

    vs_out.vertex = vertex;
    vs_out.normal = (WorldIT * vec4(normal, 0)).xyz;
    vs_out.texcoord = texcoord.xy;
    vs_out.TBN = TBN;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);

}
