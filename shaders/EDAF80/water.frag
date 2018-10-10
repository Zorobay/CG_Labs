#version 410

uniform vec3 light_position;
uniform vec3 camera_position;
uniform samplerCube reflection;
uniform float time;
uniform sampler2D bump_texture;

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 texcoord;
    vec3 binormal;
    vec3 tangent;
} fs_in;

out vec4 frag_color;

vec4 color_deep = vec4(0.0f, 0.0f, 0.1f, 1.0f);
vec4 color_shallow = vec4(0.0f, 0.5f, 0.5f, 1.0f);

float bumpTime = mod(time, 100);
vec2 texScale = vec2(8, 4);
vec2 bumpSpeed = vec2(-0.05, 0);

void main()
{
    vec2 bumpCoord0 = fs_in.texcoord.xy*texScale + bumpTime*bumpSpeed;
    vec2 bumpCoord1 = fs_in.texcoord.xy*texScale*2 + bumpTime*bumpSpeed*4;
    vec2 bumpCoord2 = fs_in.texcoord.xy*texScale*4 + bumpTime*bumpSpeed*8;

    vec4 n0 = texture(bump_texture, bumpCoord0)*2 - 1;
    vec4 n1 = texture(bump_texture, bumpCoord1)*2 - 1;
    vec4 n2 = texture(bump_texture, bumpCoord2)*2 - 1;
    vec4 n_bump = normalize(n0 + n1 + n2);

    vec3 b = normalize(fs_in.binormal);
    vec3 t = normalize(fs_in.tangent);
    vec3 n = normalize(fs_in.normal);

    mat3 BTN = mat3(b,t,n);

    vec3 view = camera_position - fs_in.vertex; //WHAT IS DIS?
    vec3 V = normalize(view);
    n = BTN * n_bump.xyz;
    vec3 R = reflect(-V, n);
    float facing = 1.0 - max(dot(V, n), 0);

    vec4 color_water = mix(color_deep, color_shallow, facing);
    vec4 reflection = texture(reflection, R);
    float R_0 = 0.02037;
    float fresnel = R_0 + (1.0-R_0)*pow((1.0 - dot(V, n)), 5);
    vec3 L = normalize(light_position - fs_in.vertex);
    vec3 refraction = refract(L, fs_in.normal, 1.33);

    frag_color = color_water + reflection*fresnel + vec4(refraction*(1.0 - fresnel), 1);
}