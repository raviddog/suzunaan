#version 330 core

layout (location = 0) in vec4 coord;

out vec2 txCoord;

uniform mat4 scrProjection;

void main()
{
    txCoord = coord.zw;
    gl_Position = scrProjection * vec4(coord.xy, 0.0, 1.0);
}