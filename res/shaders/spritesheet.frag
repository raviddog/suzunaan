#version 330 core

in vec2 txCoord;
// in vec3 angle;

uniform sampler2D txUnit;

void main()
{
    // vec2 coord = txCoord;
    // float sin_factor = sin(angle.z);
    // float cos_factor = cos(angle.z);
    // coord = vec2((coord.x - 0.5) * (angle.x / angle.y), coord.y - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);
    // // coord = (coord - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);
    // coord += 0.5;
    gl_FragColor = texture(txUnit, txCoord);
}