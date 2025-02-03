#version 450

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

void main(){
	FragColor = vec4(1.0f) - texture(screenTexture, texCoords);
}