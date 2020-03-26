#version 330 core
//gathered from here: https://learnopengl.com/In-Practice/2D-Game/Particles by (Joey De Vries)
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    color = (texture(sprite, TexCoords) * ParticleColor);
}  