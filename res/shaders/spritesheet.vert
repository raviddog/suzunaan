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

void main()
{
    txCoord = tex;
    vec2 normalized_size = 2.0 * size / res;
    vec2 normalized_coord = 2.0 * coord / res - 1.0;

    //  rotate to the right instead of the left, clockwise is more intuitive
    // float angle = -radians(rotation);

    //  steps to rotate
    //  1   get coord as size offset from 0,0
    //  2   apply rotation matrix
    //  3   move coord 

    float cosa = cos(-radians(rotation));
    float sina = sin(-radians(rotation));

    //  need local coordinate as offset from 0,0
    //  provide size as negative when needed
    // float x = (cosa * normalized_size.x) - (sina * normalized_size.y);
    // float y = (sina * normalized_size.x) + (cosa * normalized_size.y);

    vec2 rotated_coord = vec2((cosa * normalized_size.x) - (sina * normalized_size.y), (sina * normalized_size.x) + (cosa * normalized_size.y));

    //  fix rectangular distortion
    // vec2 ratio = vec2(res.y / res.x, res.x / res.y);

    //  apply amount of distortion based on rotation angle
    vec2 distortion = vec2((abs(sina) * (res.y / res.x - 1.0)) + 1.0, (abs(sina) * (res.x / res.y - 1.0)) + 1.0);
    
    //  move coord based on center destination
    gl_Position = vec4(rotated_coord * distortion + normalized_coord, 0.0, 1.0);
}