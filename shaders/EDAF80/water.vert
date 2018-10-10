#version 410

// A(sin((D*x + D*z)*f + t*p)*0.5 +0.5)^k

float amp1 = 1.0f;
float amp2 = 0.5f;
float freq1 = 0.4f;
float freq2 = 0.4f;
float phase1 = 0.5f;
float phase2 = 1.3f;
float sharpness = 2.0f;
vec3 D1 = vec3(-1.0f, 0.0f, 0.0f);
vec3 D2 = vec3(-0.7f, 0.0f, -0.7f);

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;


uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform float time;

out VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec3 texcoord;
    vec3 binormal;
    vec3 tangent;
} vs_out;

float wave(float Amp, float frequency, float phase, float sharpness, vec3 direction, float time, vec3 vertex){
    return Amp*pow((sin((direction.x*vertex.x + direction.z*vertex.z)*frequency + time*phase)*0.5 + 0.5), sharpness);
}

float diffWaveX(float Amp, float frequency, float phase, float sharpness, vec3 direction, float time, vec3 vertex){
    return 0.5*sharpness*frequency*Amp*pow((sin((direction.x*vertex.x + direction.z*vertex.z)*frequency + time*phase)*0.5 +0.5), sharpness-1.0) * cos((direction.x*vertex.x + direction.z*vertex.z)*frequency + time*phase) * direction.x;
}

float diffWaveZ(float Amp, float frequency, float phase, float sharpness, vec3 direction, float time, vec3 vertex){
    return 0.5*sharpness*frequency*Amp*pow((sin((direction.x*vertex.x + direction.z*vertex.z)*frequency + time*phase)*0.5 +0.5), sharpness-1.0) * cos((direction.x*vertex.x + direction.z*vertex.z)*frequency + time*phase) * direction.z;
}

void main()
{
    float y = wave(amp1, freq1, phase1, sharpness, D1, time, vertex) +  wave(amp2, freq2, phase2, sharpness, D2, time, vertex);
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex.x, y, vertex.z, 1.0);
	vs_out.vertex = vec3(vertex.x, y, vertex.z);

    float diffX = diffWaveX(amp1, freq1, phase1, sharpness, D1, time, vertex)
                + diffWaveX(amp2, freq2, phase2, sharpness, D2, time, vertex);
    float diffZ = diffWaveZ(amp1, freq1, phase1, sharpness, D1, time, vertex)
                + diffWaveZ(amp2, freq2, phase2, sharpness, D2, time, vertex);
    vs_out.normal = vec3(-diffX, 1, -diffZ);
    vs_out.texcoord = texcoord;
    vs_out.binormal = vec3(1, diffX, 0);
    vs_out.tangent = vec3(0, diffZ, 1);

}
