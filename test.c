#include <stdio.h>

#define VEC2_IMPLEMENTATION
#include "vec2.h"

#define PI 3.14159265359

vec2 polar2cartesian(vec2 v_in);
vec2 cartesian2polar(vec2 v_in);

int main(int argc, char const *argv[])
{
    vec2 position_c = vec2_new(1,1);
    VEC2_PRINT(position_c);
    vec2 position_p = cartesian2polar(position_c);
    VEC2_PRINT(position_p);
    VEC2_PRINT(polar2cartesian(position_p));

    float x = 40.5;

    vec2 v1 = vec2_new(0,1);
    VEC2_PRINT(v1);
    VEC2_PRINT(polar2cartesian(v1));
    printf("%10.3f\n", x);

    return 0;
}


vec2 polar2cartesian(vec2 v_in)
{
    float theta = v_in.y;
    float length = v_in.x;
    vec2 v_out = {
        .x = length*cosf(theta),
        .y = length*sinf(theta)
    };
    return v_out;
}

vec2 cartesian2polar(vec2 v_in)
{
    float theta = atan2f(v_in.y, v_in.x);
    float length = vec2_length(&v_in);
    vec2 v_out = {
        .x = length,
        .y = theta
    };
    return v_out;
}