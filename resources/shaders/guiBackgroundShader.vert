#version 400 core

uniform struct Matrices{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inTexture; //NOT USED
layout (location = 2) in vec3 inNormal; //NOT USED

void main(){	
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
} 
	