#version 330 core

layout (location = 0) in vec4 coord;

out vec2 txCoord;

uniform mat4 scrProjection;
uniform mat4 scrModel;
uniform mat4 scrScale;

void main()
{
    txCoord = coord.zw;
    gl_Position = scrProjection * scrModel * scrScale * vec4(coord.xy, 0.0, 1.0);
    //gl_Position = vec4(coord.xy, 0.0, 1.0);
}