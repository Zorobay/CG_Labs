#version 410

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 texcoord;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform float t;

// Define constants
uniform float amp1;
uniform float amp2;
float freq1 = 0.4;
float freq2 = 0.4;
float phase1 = 0.5;
float phase2 = 1.5;
float sharpness1 = 3.0;
float sharpness2 = 5.0;
vec2 dir1 = vec2(-1.0, 0.0);
vec2 dir2 = vec2(-0.7, 0.7);

// Define output
out VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 texcoord;
    float diff_sum_x;
    float diff_sum_z;
} vs_out;


float wave(in float amplitude, in float freq, in float phase, in float sharpness, in vec2 dir, in float x, in float z, in float t)
{
    float y = amplitude * pow(sin((dir.x * x + dir.y * z) * freq + t*phase)*0.5f + 0.5f, sharpness);
    return(y);
}

float diff_wave_x(in float amplitude, in float freq, in float phase, in float sharpness, in vec2 dir, in float x, in float z, in float t)
{
    float y = 0.5 * sharpness * freq * amplitude * pow(sin((dir.x * x + dir.y * z) * freq + t*phase)*0.5f + 0.5f, sharpness- 1) *
                                                        cos((dir.x * x + dir.y * z) * freq + t*phase)*dir.x;
    return(y);
}

float diff_wave_z(in float amplitude, in float freq, in float phase, in float sharpness, in vec2 dir, in float x, in float z, in float t)
{
    float y = 0.5 * sharpness * freq * amplitude * pow(sin((dir.x * x + dir.y * z) * freq + t*phase)*0.5f + 0.5f, sharpness- 1) *
                                                    cos((dir.x * x + dir.y * z) * freq + t*phase)*dir.y;
    return(y);
}

void main()
{
    // Calculate wave displacement in y
    vec3 new_vertex;
    new_vertex.xz = vertex.xz;
    new_vertex.y = wave(amp1, freq1, phase1, sharpness1, dir1, vertex.x, vertex.z, t) +
                    wave(amp2, freq2, phase2, sharpness2, dir2, vertex.x, vertex.z, t);
    //new_vertex = (vertex_model_to_world * vec4(new_vertex, 1)).xyz;
    vs_out.vertex = new_vertex;

    // Calculate differentials to get normal
    float diff_sum_x = diff_wave_x(amp1, freq1, phase1, sharpness1, dir1, vertex.x, vertex.z, t) +
                    diff_wave_x(amp2, freq2, phase2, sharpness2, dir2, vertex.x, vertex.z, t);
    float diff_sum_z = diff_wave_z(amp1, freq1, phase1, sharpness1, dir1, vertex.x, vertex.z, t) +
                    diff_wave_z(amp2, freq2, phase2, sharpness2, dir2, vertex.x, vertex.z, t);

    vs_out.normal = vec3(-diff_sum_x, 1.0, -diff_sum_z);
    vs_out.texcoord = texcoord;
    vs_out.diff_sum_x = diff_sum_x;
    vs_out.diff_sum_z = diff_sum_z;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(new_vertex, 1.0);
}



