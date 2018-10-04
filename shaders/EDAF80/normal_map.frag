#version 410

uniform vec3 light_position;
uniform vec3 camera_position;

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
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

uniform sampler2D bump_texture;
uniform sampler2D diffuse_texture;

out vec4 frag_color;

void main()
{

    vec3 view = camera_position - fs_in.vertex; //WHAT IS DIS?
    vec3 light = light_position - fs_in.vertex;

    // Construct TBN matrix for normal mapping
    vec3 T = normalize(fs_in.tangent);
    vec3 B = normalize(fs_in.binormal);
    vec3 N = normalize(fs_in.normal);
    mat3 TBN = mat3(T, B, N);
    vec3 Diffuse = texture(diffuse_texture, fs_in.texcoord).xyz;

    vec3 normal_map_normal = texture(bump_texture, fs_in.texcoord).xyz;
    normal_map_normal = normalize(normal_map_normal * 2.0 - 1.0); //convert back to -1 to 1
    vec3 tnbed = TBN * normal_map_normal;
    normal_map_normal = normalize((normal_model_to_world * vec4(tnbed, 1)).xyz);
    vec3 normal = normal_map_normal;
//    vec3 frag_norm = texture(normalMap, fs_in.texcoord).rgb; // Fetch normal vector value from normal map
//    frag_norm = normalize(frag_norm * 2.0 - 1.0); //Convert back to [0,1]
//    frag_norm = normalize(fs_in.TBN * frag_norm); // Convert to correct coordinate system

    //WHY DO WE NEED TO RENORMALIZE?
    vec3 L = normalize(light); //Normalize light vector
    vec3 V = normalize(view); //Normalize view vector
    vec3 R = normalize(reflect(-L, normal)); //Normalize reflection vector
    vec3 DIFFUSE = Diffuse * max(dot(normal, L), 0.0);
    vec3 SPECULAR = specular * pow(max(dot(R,V), 0.0), shininess);

    frag_color.xyz = ambient + DIFFUSE + SPECULAR;
    frag_color.w = 1.0;

    //frag_color = vec4(1.0, 0, 0, 1.0);
}
