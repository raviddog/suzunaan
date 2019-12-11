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
// uniform mat4 r;
uniform vec2 res;

void main()
{
    // angle = rotation;
    txCoord = tex;
    vec2 normalized_size = vec2((2.0 / res.x) * size.x, (2.0 / res.y) * size.y);
    vec2 normalized_coord = vec2(((2.0 / res.x) * coord.x) - 1.0, ((2.0 / res.y) * coord.y) - 1.0);
    if(rotation == 0.0) {
        gl_Position = vec4(normalized_coord + normalized_size, 0.0, 1.0);
    } else {
        //  steps to rotate
        //  1   get coord as size offset from 0,0
        //  2   apply rotation matrix
        //  3   move coord 

        //  need local coordinate as offset from 0,0
        //  provide size as negative when needed
        // vec2 center_coord = vec2(normalized_size / 2);
        float x = (cos(rotation) * normalized_size.x) - (sin(rotation) * normalized_size.y);
        float y = (sin(rotation) * normalized_size.x) + (cos(rotation) * normalized_size.y);

        vec2 rotated_coord = vec2(x, y);

        //  fix rectangular distortion
        vec2 ratio = vec2(res.y / res.x, res.x / res.y);

        //  calculate distortion applied in each direction
        // vec2 percentage = vec2()

        //  apply amount of distortion based on rotation angle
        vec2 distortion = vec2((abs(sin(rotation)) * (ratio.x - 1.0)) + 1.0, (abs(sin(rotation)) * (ratio.y - 1.0)) + 1.0);

        rotated_coord = vec2(rotated_coord.x * distortion.x, rotated_coord.y * distortion.y);
        
        //  move coord based on center destination
        vec2 final_coord = rotated_coord + normalized_coord;
        gl_Position = vec4(final_coord, 0.0, 1.0);


        //  converts coordinate to normalized
        // vec2 normalized_coords = vec2(((2.0 / res.x) * coord.x) - 1.0, ((2.0 / res.y) * coord.y) - 1.0);
        // //  converts size to normalized
        // vec2 normalized_size = vec2((2.0 / res.x) * size.x, (2.0 / res.y) * size.y);
        // //  coordinate centered
        
        // // vec4 centered_coords = vec4(normalized_size / 2.0, 0.0, 1.0);

        // vec2 normalized_coords_zero = vec2(((2.0 / res.x) * coord.x), ((2.0 / res.y) * coord.y));

        // vec4 centered_coords = vec4(((2.0 / res.x) * normalized_size.x) - 1.0, ((2.0 / res.y) * normalized_size.y) - 1.0, 0.0, 1.0);
        // vec4 rotated_centered_coords = r * centered_coords;
        

        // gl_Position = vec4(rotated_centered_coords.xy + normalized_coords.xy, 0.0, 1.0);


        // float sina = sin(rotation);
        // float cosa = cos(rotation);
        // mat4 rotate;
        // rotate[0] = vec4(cosa, sina, 0.0, 0.0);
        // rotate[1] = vec4(-sina, cosa, 0.0, 0.0);
        // rotate[2] = vec4(0.0, 0.0, 1.0, 0.0);
        // rotate[3] = vec4(0.0, 0.0, 0.0, 1.0);
        // vec4 centered_rotated_coords = r * centered_coords;
        //vec2 final_coords = vec2(centered_rotated_coords.x - 1, centered_rotated_coords.y - 1);

        //coord2 = vec2((coord.x - 0.5) * (rotation.x / rotation.y), coord.y - 0.5) * mat2(cosa, sina, -sina, cosa);
        // coord = (coord - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);

        //vec4 opos = scrProjection * vec2(coord2, 0.0, 1.0);
        // gl_Position = vec4(centered_rotated_coords.x + normalized_coords.x, centered_rotated_coords.y + normalized_coords.y, 0.0, 1.0);
    }
}