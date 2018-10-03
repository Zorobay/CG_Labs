#version 410

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    mat3 TBN;
    vec3 view;
    vec3 light;
	vec2 texcoord;
	//mat3 TBN;
} fs_in;

uniform vec3 ambient; // Material ambient
uniform vec3 diffuse; // Material diffuse
uniform vec3 specular; // Material specular
uniform float shininess;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

uniform sampler2D normalMap;
// uniform sampler2D texture;

out vec4 frag_color;

void main()
{
    vec3 normal_map_normal = texture(normalMap, fs_in.texcoord).rgb;
    normal_map_normal = normalize(normal_map_normal * 2.0 - 1.0); //convert back to -1 to 1
    vec3 tnbed = fs_in.TBN * normal_map_normal;
    normal_map_normal = normalize((normal_model_to_world * vec4(tnbed, 1)).xyz);
    vec3 normal = normal_map_normal;
//    vec3 frag_norm = texture(normalMap, fs_in.texcoord).rgb; // Fetch normal vector value from normal map
//    frag_norm = normalize(frag_norm * 2.0 - 1.0); //Convert back to [0,1]
//    frag_norm = normalize(fs_in.TBN * frag_norm); // Convert to correct coordinate system

    //WHY DO WE NEED TO RENORMALIZE?
    vec3 N = normalize(fs_in.normal); //Normalized normal vector fetched from normal map
    vec3 L = normalize(fs_in.light); //Normalize light vector
    vec3 V = normalize(fs_in.view); //Normalize view vector
    vec3 R = normalize(reflect(-L, N)); //Normalize reflection vector
    vec3 DIFFUSE = diffuse * max(dot(N, L), 0.0);
    vec3 SPECULAR = specular * pow(max(dot(R,V), 0.0), shininess);

    frag_color.xyz = ambient + DIFFUSE + SPECULAR;
    frag_color.w = 1.0;

    //frag_color = vec4(1.0, 0, 0, 1.0);
}
