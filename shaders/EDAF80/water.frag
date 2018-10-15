#version 410

out vec4 frag_color;

uniform float t;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform samplerCube reflection_cube_map;
uniform sampler2D bump_map;
uniform vec4 color_deep;
uniform vec4 color_shallow;

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 texcoord;
    float diff_sum_x;
    float diff_sum_z;
} fs_in;

// Define constants


vec2 texScale = vec2(8, 4);
float bumpTime = mod(t, 100.0); // bumpTime 0-100
vec2 bumpSpeed = vec2(-0.05, 0);

void main()
{
    // Calculate sliding coordinates from bump map

    vec2 bump1 = fs_in.texcoord.xy*texScale + bumpTime*bumpSpeed; //WHAT?
    vec2 bump2 = fs_in.texcoord.xy*texScale*2 + bumpTime*bumpSpeed*4;
    vec2 bump3 = fs_in.texcoord.xy*texScale*4 + bumpTime*bumpSpeed*8;

    vec3 n1 = texture(bump_map, bump1).rgb * 2 - 1;
    vec3 n2 = texture(bump_map, bump2).rgb * 2 - 1;
    vec3 n3 = texture(bump_map, bump3).rgb * 2 - 1;
    vec3 n_bump = normalize(n1 + n2 + n3);

    vec3 b = normalize(vec3(1, fs_in.diff_sum_x, 0));
    vec3 t = normalize(vec3(0, fs_in.diff_sum_z, 1));
    mat3 BTN = mat3(b, t, normalize(fs_in.normal));

    //Calculate water color
    vec3 V = normalize(camera_position - fs_in.vertex); // View vector
    vec3 N = BTN * n_bump;
    float facing = 1.0 - max(dot(V, N), 0);
    vec4 water_color = mix(color_deep, color_shallow, facing);

    // Calculate reflection
    vec3 R = reflect(-V, N);
    vec4 reflection_color = texture(reflection_cube_map, R);

    // Calculate fresnel term
    float R0 = 0.02037; // air to water
    float fastFresnel = R0 + (1.0 - R0) * pow((1.0 - dot(V, N)), 5);

    // Refraction
    vec3 incidence = normalize(light_position - fs_in.vertex);
    vec3 Re = refract(-V, N, 1/1.33); // RÄTT NORMAL!? Rätt incidence??
    vec3 refraction = texture(reflection_cube_map, Re).xyz;
    vec4 refraction_color = vec4(refraction * (1 - fastFresnel), 1);

	frag_color = water_color + reflection_color * fastFresnel + refraction_color;
}
