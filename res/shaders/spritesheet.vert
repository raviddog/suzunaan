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
    //  1   get coord as size offset from 0,0
    //  1.5     apply distortion to unstretch diagonal sprites
    //  2   apply rotation matrix
    //  2.5     apply distortion to correct sprite to screen aspect ratio
    //  3   move coord

    //  steps to rotate
    //  1   get coord as size offset from 0,0
    //  2   calculate initial angle from origin
    //  3   add rotation angle
    //  4   move coord

    //  normalize coordinates
    //  first normalize size to vertical square
    vec2 normalized_size = size / res.yy;
    vec2 normalized_coord = 2.0 * coord / res - 1.0;

    // float angle = atan(normalized_size.x / normalized_size.y);
    // float length = sqrt(normalized_coord.x * normalized_coord.x + normalized_coord.y * normalized_coord.y);
    // angle += radians(rotation);
    // float x = length * cos(angle);
    // float y = length * sin(angle);
    // x = x * res.x / res.y;
    // gl_Position = vec4(x + normalized_coord.x, y + normalized_coord.y, 0.0, 1.0);

    //  rotate to the right instead of the left, clockwise is more intuitive
    float cosa = cos(-radians(rotation));
    float sina = sin(-radians(rotation));

    // float theta = atan(normalized_coord.y / normalized_coord.x);
    // vec2 rotated_coord = vec2(length * cos(theta + rotation), length * sin(theta + rotation));



    //  need local coordinate as offset from 0,0
    //  provide size as negative when needed
    // float x = (cosa * normalized_size.x) - (sina * normalized_size.y);
    // float y = (sina * normalized_size.x) + (cosa * normalized_size.y);

    //  apply distortion prior to rotating to unstretch diagonal sprites
    //  apply based on how close to a 45 degree angle it is
    // float t = abs(sina) * abs(cosa);
    // vec2 pre_distortion = abs(sina) * (res.xy / res.yx - 1.0) + 1.0;
    // float pre_distortion_ratio = mix(abs(sin(-radians(rotation * 2))), 0.0, distortion_amount);
    // vec2 pre_distortion_corrected = vec2(mix(1.0, pre_distortion.x, pre_distortion_ratio), mix(1.0, pre_distortion.y, pre_distortion_ratio));

    // normalized_size = pre_distortion_corrected * normalized_size;
    vec2 rotated_coord = vec2((cosa * normalized_size.x) - (sina * normalized_size.y), (sina * normalized_size.x) + (cosa * normalized_size.y));

    // // //  distortion to correct rotation to screen aspect ratio
    // vec2 post_distortion = abs(sina) * (res.yx / res.xy - 1.0) + 1.0;
    vec2 post_distortion = res.yx / res.xx;
    // vec2 post_distortion = vec2(mix(res.y / res.x, 1.0, 0.5), mix(res.x / res.y, 1.0, 0.5));
    
    // //  move coord based on center destination
    gl_Position = vec4(rotated_coord * post_distortion + normalized_coord, 0.0, 1.0);
}