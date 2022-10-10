#pragma once
#include "ray.h"

class aabb
{
public:
	aabb(){};
	aabb(vec::vec3 _min, vec::vec3 _max) : _min(_min), _max(_max){};

	vec::vec3 min() const { return _min; }
	vec::vec3 max() const { return _max; }
	bool hit(const ray &_ray, float t_min, float t_max) const;

	vec::vec3 _min, _max; // left lower cornor, top right
};

inline bool aabb::hit(const ray &_ray, float t_min, float t_max) const
{
	for (int i = 0; i < 3; i++)
	{
		float invD = 1.0f / _ray.direction()[i];
		float t0 = (min()[i] - _ray.origin()[i]) * invD;
		float t1 = (max()[i] - _ray.origin()[i]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);
		t_min = t0 > t_min ? t0 : t_min;
		t_max = t1 < t_max ? t1 : t_max; // wrong write: t_max = t0 > t_max ? t0 : t_max; lead to lower than before
		if (t_max <= t_min)
			return false;
	}
	return true;
}

inline aabb surrounding_box(aabb box0, aabb box1)
{
	vec::vec3 small(std::min(box0.min().x(), box1.min().x()),
					std::min(box0.min().y(), box1.min().y()),
					std::min(box0.min().z(), box1.min().z()));
	vec::vec3 big(std::max(box0.max().x(), box1.max().x()),
				  std::max(box0.max().y(), box1.max().y()),
				  std::max(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}