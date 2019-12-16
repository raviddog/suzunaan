#version 330 core

//  screen coords
//  texture coords
//  normalized size coordinate
//  rotation angle

layout (location = 0) in vec2 coord;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec2 size;
layout (location = 3) in float rotation;

out vec2 txCoord;

uniform mat4 scrProjection;
uniform vec2 res;
uniform float distortion_amount;

void main()
{
    txCoord = tex;

    //  steps to rotate
    //  1   get size offset from 0,0, normalised
    //  2   rotate
    //  3   apply window aspect ratio
    //  4   move coord to desired position

    //  normalize coordinates
    //  first normalize size to vertical square
    vec2 normalized_size = size / res.yy;
    vec2 normalized_coord = 2.0 * coord / res - 1.0;

    //  rotate to the right instead of the left, clockwise is more intuitive
    float cosa = cos(-radians(rotation));
    float sina = sin(-radians(rotation));

    //  rotate
    vec2 rotated_coord = vec2((cosa * normalized_size.x) - (sina * normalized_size.y), (sina * normalized_size.x) + (cosa * normalized_size.y));

    //  apply window aspect ratio post-rotation
    vec2 distortion = res.yx / res.xx;
    
    // //  move coord based on center destination
    gl_Position = vec4(rotated_coord * distortion + normalized_coord, 0.0, 1.0);
}