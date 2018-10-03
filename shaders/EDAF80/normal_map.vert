#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 binormal;

uniform vec3 light_position;
uniform vec3 camera_position;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
    vec3 vertex;
    vec3 normal;
    mat3 TBN;
    vec3 view;
    vec3 light;
	vec2 texcoord;
	//mat3 TBN;
} vs_out;


void main()
{


    // Construct TBN matrix for normal mapping
    vec3 T = normalize(tangent);
    vec3 B = normalize(binormal);
    vec3 N = normalize(normal);
    mat3 TBN = mat3(T, B, N);

    // Calculate light and view stuffs
    vec3 world_position = (vertex_model_to_world * vec4(vertex, 1)).xyz;
    vs_out.view = camera_position - world_position; //WHAT IS DIS?
    vs_out.light = light_position - world_position;
    vs_out.normal = (normal_model_to_world * vec4(normal, 1)).xyz;
    vs_out.vertex = world_position;
    vs_out.TBN = TBN;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);

}
