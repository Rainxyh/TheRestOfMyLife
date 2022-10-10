#pragma once
#include "vec3.h"
class onb
{
public:
    onb(){};

    inline vec::vec3 operator[](int i) const { return axis[i]; }
    inline vec::vec3 u() const { return axis[0]; }
    inline vec::vec3 v() const { return axis[1]; }
    inline vec::vec3 w() const { return axis[2]; }
    inline vec::vec3 local(float a, float b, float c) const { return a * u() + b * v() + c * w(); }
    inline vec::vec3 local(const vec::vec3 &a) const { return a.x() * u() + a.y() * v() + a.z() * w(); }
    void build_from_w(const vec::vec3 &n);

    vec::vec3 axis[3];
};

void onb::build_from_w(const vec::vec3 &n)
{
    axis[2] = vec::unit_vector(n);
    vec::vec3 a;
    if (fabs(w().x()) > 0.9) // if input vector so close to x axis, choose y axis, sb bug
        a = vec::vec3(0, 1, 0);
    else
        a = vec::vec3(1, 0, 0);
    axis[1] = vec::unit_vector(vec::cross(w(), a));
    axis[0] = vec::cross(v(), w());  // right hand coordinate system
}
