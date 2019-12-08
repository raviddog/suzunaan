#version 330 core

in vec2 txCoord;
out vec4 color;

uniform mat4 txProjection;
uniform sampler2D txUnit;

void main()
{
    //color = texture(txUnit, vec4(txProjection * vec4(txCoord, 0.0, 0.0)).xy);
    color = texture(txUnit, txCoord);
}