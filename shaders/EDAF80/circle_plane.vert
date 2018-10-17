#version 410

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 texcoord;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform float max_radius;

out VS_OUT {
	vec2 texcoord;
	vec3 position;
	float max_radius;
} vs_out;


void main()
{
	vs_out.texcoord = vec2(texcoord.x, texcoord.y);

	vs_out.position = vertex;
	vs_out.max_radius = max_radius;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}
