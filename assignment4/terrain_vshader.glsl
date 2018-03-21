R"(
#version 330 core
uniform sampler2D noiseTex;

in vec3 vposition;
in vec2 vtexcoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 uv;
out vec3 fragPos;

void main() {
    /// TODO: Get height h at uv


    uv = vtexcoord;
    float h = texture(noiseTex, uv).r;
    fragPos = vposition.xyz + vec3(0,0,h);
    gl_Position = P*V*M*vec4(fragPos, 1.0);
}
)"