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
    vec2 normalized_size = vec2((2.0 / res.x) * size.x, (2.0 / res.y) * size.y);
    vec2 normalized_coord = vec2(((2.0 / res.x) * coord.x) - 1.0, ((2.0 / res.y) * coord.y) - 1.0);
    float angle = radians(rotation);
    
    if(angle == 0.0) {
        gl_Position = vec4(normalized_coord + normalized_size, 0.0, 1.0);
    } else {
        //  steps to rotate
        //  1   get coord as size offset from 0,0
        //  2   apply rotation matrix
        //  3   move coord 

        //  need local coordinate as offset from 0,0
        //  provide size as negative when needed
        float x = (cos(angle) * normalized_size.x) - (sin(angle) * normalized_size.y);
        float y = (sin(angle) * normalized_size.x) + (cos(angle) * normalized_size.y);

        vec2 rotated_coord = vec2(x, y);

        //  fix rectangular distortion
        vec2 ratio = vec2(res.y / res.x, res.x / res.y);

        //  apply amount of distortion based on rotation angle
        vec2 distortion = vec2((abs(sin(angle)) * (ratio.x - 1.0)) + 1.0, (abs(sin(angle)) * (ratio.y - 1.0)) + 1.0);
        rotated_coord = vec2(rotated_coord.x * distortion.x, rotated_coord.y * distortion.y);
        
        //  move coord based on center destination
        gl_Position = vec4(rotated_coord + normalized_coord, 0.0, 1.0);
    }
}