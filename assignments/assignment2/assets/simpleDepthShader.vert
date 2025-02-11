#version 450

layout (location = 0) in vec3 aPos;

mat4 lightSpaceMatrix;
mat4 model;

void main(){
    gl_Position = lightSpaceMatrix * model * vec4(aPos,1.0);
}