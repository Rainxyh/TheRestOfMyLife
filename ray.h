#pragma once
#include "vec3.h"
class ray
{
public:
	ray()
	{
		ori = 0.0;
		dir = vec::vec3();
		time = 0.0f;
	};
	ray(const vec::vec3 &ori, const vec::vec3 &dir) : ori(ori), dir(dir){};
	ray(const vec::vec3 &ori, const vec::vec3 &dir, float time) : ori(ori), dir(dir), time(time){};

	vec::vec3 origin() const { return ori; }
	vec::vec3 direction() const { return dir; }
	float get_time() const { return time; }
	vec::vec3 point_at_parameter(float t) const { return vec::vec3(ori + dir * t); }

	vec::vec3 ori, dir;
	float time;
};

inline vec::vec3 reflect(const vec::vec3 &v, const vec::vec3 &n)
{
	return v - 2 * dot(v, n) * n;
}

inline bool refract(const vec::vec3 &v, const vec::vec3 &normal, float ni_over_nt, vec::vec3 &scattered)  // big angle ni_over_nt<1, small angle ni_over_nt>1
{
	vec::vec3 uv = unit_vector(v);
	float cosine = dot(uv, normal);
	float discrimination = 1.0f - ni_over_nt * ni_over_nt * (1.0f - cosine * cosine);
	if (discrimination > 0)  // refract
	{
		vec::vec3 refracted = ni_over_nt * (uv - normal * cosine) - normal * sqrt(discrimination);
		scattered = refracted;
		return true;
	}  // else full reflect
	vec::vec3 reflected = reflect(v, normal);
	scattered = reflected;
	return false;
}
