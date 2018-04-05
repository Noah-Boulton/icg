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

    //Use normal to find the slope

    /// TODO: Texture according to height and slope
    /// HINT: Read noiseTex for height at uv

    /// TODO: Calculate ambient, diffuse, and specular lighting
    /// HINT: max(,) dot(,) reflect(,) normalize()

    //color = vec4(0,0,0,1);
    float specular_power;
    float specular_colour;
    float ambient_light = 0.5f;
    vec3 colour = vec3(0,0,0);
    float snow_line = 0.65f;
    float tree_line = 0.5f;
    if(fragPos.z > snow_line){
	colour = texture(snow, uv).rgb;
	specular_power = 125.0f;
    } else if (fragPos.z > tree_line){
	colour = texture(rock, uv).rgb;
	specular_power = 10.0f;
    }else if(fragPos.z <= 0.0f){
	colour = texture(water, uv).rgb;
	specular_power = 500.0f;
	//vec3 normal_col = normalize(abs((2.0f*colour)-1.0f));
    } else{
	colour = texture(grass, uv).rgb;
	specular_power = 10.0f;
    }

    float angle = dot(N, vec3(0,1,0));

    if(abs(angle) > 0.95f && fragPos.z > snow_line){
	colour = texture(rock, uv).rgb;
	specular_power = 10.0f;
    }

    vec3 light_color = vec3(0.8, 0.9, 0.9);
    float d_weight = max(-dot(lightDir, N), 0);
    float s_weight = pow(max(dot(normalize(reflect(viewPos, N)), lightDir), 0), specular_power);

    vec3 ambient = colour * ambient_light;
    vec3 diffuse = d_weight * light_color * colour;
    vec3 specular = s_weight * light_color * colour;

    //vec3 illumination = colour * (ambient_light + d_weight + s_weight);

    vec3 illumination;
    if(d_weight > 0.0f){
	illumination = ambient + diffuse + specular;
    } else {
	illumination = ambient + diffuse;
    }
    color = vec4(illumination, 1);
}
)"
