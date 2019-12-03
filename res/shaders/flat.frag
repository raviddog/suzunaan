#version 330 core

in vec2 tex;
out vec4 FragColor;

uniform sampler2D tx;

void main()
{
    FragColor = texture(tx, tex);
}
