R"(
#version 330 core
uniform sampler2D noiseTex;

uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D sand;
uniform sampler2D snow;
uniform sampler2D water;

// The camera position
uniform vec3 viewPos;

in vec2 uv;
// Fragment position in world space coordinates
in vec3 fragPos;

out vec4 color;

void main() {

    // Directional light source
    vec3 lightDir = normalize(vec3(1,1,1));

    // Texture size in pixels
    ivec2 size = textureSize(noiseTex, 0);

    /// TODO: Calculate surface normal N
    /// HINT: Use textureOffset(,,) to read height at uv + pixelwise offset
    /// HINT: Account for texture x,y dimensions in world space coordinates (default f_width=f_height=5)
    vec3 A = vec3(uv.x +1.0/size.x, uv.y, textureOffset(noiseTex, uv, ivec2(1,0)));
    vec3 B = vec3(uv.x -1.0/size.x, uv.y, textureOffset(noiseTex, uv, ivec2(-1,0)));
    vec3 C = vec3(uv.x, uv.y +1.0/size.y, textureOffset(noiseTex, uv, ivec2(0,1)));
    vec3 D = vec3(uv.x, uv.y -1.0/size.y, textureOffset(noiseTex, uv, ivec2(0,-1)));
    vec3 N = normalize( cross(normalize(A-B), normalize(C-D)) );

    /// TODO: Texture according to height and slope
    /// HINT: Read noiseTex for height at uv

    /// TODO: Calculate ambient, diffuse, and specular lighting
    /// HINT: max(,) dot(,) reflect(,) normalize()

    //color = vec4(0,0,0,1);
    //please: (make) mountains*30;
    vec4 colour = vec4(0,0,0,0);
    if(fragPos.z > 3.3f){
	colour = texture(snow, uv).rgba;
    } else if(fragPos.z < 2.7f){
	colour = texture(water, uv).rgba;
    } else{
	colour = texture(rock, uv).rgba;
    }

    vec3 light_color = vec3(1.0, 0.9, 0.8);

    float d_weight = max(-dot(lightDir, N), 0);
    float s_weight = pow(max(dot(reflect(viewPos, N), lightDir), 0),
			 1.0f);

    vec3 illumination = 0.2f * colour.xyz
		      + d_weight * light_color * colour.xyz
		      + s_weight * light_color * 1.0f;

    color = vec4(illumination, 1);
}
)"