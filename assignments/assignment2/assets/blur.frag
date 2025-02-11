#version 450
out vec4 FragColor;
in vec2 texCoords;
uniform sampler2D screenTexture;

const float offsetX = 1.0f / 1080.0f;
const float offsetY = 1.0f / 720.0f;

vec2 offsets[9] = vec2[]
(
    // left                       // center           // right
	vec2(-offsetX, offsetY),  vec2( 0.0f, offsetY),  vec2( offsetX, offsetY),  // top
	vec2(-offsetX, 0.0f),     vec2( 0.0f, 0.0f),     vec2( offsetX, 0.0f),     // middle
	vec2(-offsetX, -offsetY), vec2( 0.0f, -offsetY), vec2( offsetX, -offsetY)  // bottom
);

float blur[9] = float[]
(
	0.0625f,  2 * 0.0625f,  0.0625f,
	2 * 0.0625f,   4 * 0.0625f,  2 * 0.0625f,
	0.0625f,  2 * 0.0625f,  0.0625f
);

void main(){
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++){
		color += vec3(texture(screenTexture, texCoords.st + offsets[i])) * blur[i];
	}
	FragColor = vec4(color, 1.0f);
	
}