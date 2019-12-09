#version 330 core

in vec2 txCoord;
out vec4 color;

uniform sampler2D txUnit;

void main()
{
    color = texture(txUnit, txCoord);
}