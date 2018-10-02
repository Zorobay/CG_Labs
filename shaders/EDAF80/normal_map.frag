#version 410

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 view;
    vec3 light;
	vec2 texcoord;
	mat3 TBN;
} fs_in;

uniform vec3 ka; // Material ambient
uniform vec3 kd; // Material diffuse
uniform vec3 ks; // Material specular
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D texture;

out vec4 frag_color;

void main()
{

//    vec3 frag_norm = texture(normalMap, fs_in.texcoord).rgb; // Fetch normal vector value from normal map
//    frag_norm = normalize(frag_norm * 2.0 - 1.0); //Convert back to [0,1]
//    frag_norm = normalize(fs_in.TBN * frag_norm); // Convert to correct coordinate system

    vec3 N = normalize(fs_in.normal); //Normalized normal vector fetched from normal map
    vec3 L = normalize(fs_in.light); //Normalize light vector
    vec3 V = normalize(fs_in.view); //Normalize view vector
    vec3 R = normalize(reflect(-L, N)); //Normalize reflection vector
    vec3 diffuse = kd * max(dot(N,L),0.0);
    vec3 specular = ks * pow(max(dot(R,V), 0.0), shininess);

    frag_color.xyz = ka;//+ diffuse + specular;
    frag_color.w = 1.0;

    //frag_color = vec4(1.0, 0, 0, 1.0);
}
