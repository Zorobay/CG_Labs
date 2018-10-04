#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 binormal;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
	vec2 texcoord;
	//mat3 TBN;
} vs_out;


void main()
{
    // Calculate light and view stuffs
    vec3 world_position = (vertex_model_to_world * vec4(vertex, 1)).xyz;
    vs_out.vertex = world_position;
    vs_out.normal = normal;
    vs_out.binormal = binormal;
    vs_out.tangent = tangent;
    vs_out.texcoord = texcoord;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);

}
