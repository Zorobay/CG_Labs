#version 410

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    mat3 TBN;
	vec2 texcoord;
} fs_in;

uniform vec3 ambient; // Material ambient
uniform vec3 diffuse; // Material diffuse
uniform vec3 specular; // Material specular
uniform float shininess;

uniform vec3 light_position;
uniform vec3 camera_position;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out vec4 frag_color;

void main()
{

   // Construct TBN matrix for normal mapping
    vec3 T = normalize(fs_in.tangent);
    vec3 B = normalize(fs_in.binormal);
    vec3 N = normalize(fs_in.normal);
    mat3 TBN = mat3(T, B, N);

    //WHY DO WE NEED TO RENORMALIZE?
    N = normalize(fs_in.normal);
    vec3 V = normalize(camera_position - fs_in.vertex);
    vec3 L = normalize(light_position - fs_in.vertex);
    vec3 R = normalize(reflect(-L, N)); //Normalize reflection vector
    vec3 DIFFUSE = diffuse * max(dot(N, L), 0.0);
    vec3 SPECULAR = specular * pow(max(dot(R,V), 0.0), shininess);

    frag_color.xyz = ambient + DIFFUSE + SPECULAR;
    frag_color.w = 1.0;

    //frag_color = vec4(1.0, 0, 0, 1.0);
}
