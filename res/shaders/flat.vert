#version 330 core

layout (location = 0) in vec2 coord;
layout (location = 1) in vec2 texCoord;

out vec2 tex;

void main()
{
    gl_Position = vec4(coord, 0.0, 1.0);
    tex = texCoord;
}
