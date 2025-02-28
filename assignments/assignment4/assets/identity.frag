#version 450

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

const float offsetX = 1.0f / 1080.0f;
const float offsetY = 1.0f / 720.0f;

vec2 offsets[9] = vec2[]
(
	vec2(-offsetX, offsetY),  vec2( 0.0f, offsetY),  vec2( offsetX, offsetY),
	vec2(-offsetX, 0.0f),     vec2( 0.0f, 0.0f),     vec2( offsetX, 0.0f),
	vec2(-offsetX, -offsetY), vec2( 0.0f, -offsetY), vec2( offsetX, -offsetY)
);

float identity[9] = float[]
(
	0,  0,  0,
	0,   1,  0,
	0,  0,  0
);

void main(){
	vec3 color = vec3(0.0f);
	float kernel[9] = identity;
	for (int i = 0; i < 9; i++){
		color += vec3(texture(screenTexture, texCoords.st + offsets[i])) * kernel[i];
	}
	FragColor = vec4(color, 1.0f);
	
}