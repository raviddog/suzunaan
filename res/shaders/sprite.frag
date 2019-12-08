#version 330 core

in vec2 txCoord;
out vec4 color;

uniform mat4 txProjection;
uniform sampler2D txUnit;

void main()
{
    //color = vec4(texture(image, txProjection * txCoord));
    color = texture(txUnit, txCoord);
}