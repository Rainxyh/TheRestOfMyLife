#pragma once
#include "hitable.h"
#include "rand.h"
#include <memory>

class hitable_list : public hitable
{
public:
	hitable_list(){};
	hitable_list(std::shared_ptr<hitable>* list, int list_size) : list(list), list_size(list_size){};
	
	virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &bbox) const;
	float pdf_value(const vec::vec3 &o, const vec::vec3 &v) const;
	vec::vec3 random(const vec::vec3 &o) const;

	std::shared_ptr<hitable>* list;
	int list_size;
};

float hitable_list::pdf_value(const vec::vec3 &o, const vec::vec3 &v) const
{
	float weight = 1.0 / list_size;
	float sum = 0;
	for (int i = 0; i < list_size; i++)
	{
		sum += weight * list[i]->pdf_value(o, v);
	}
	return sum;
}

vec::vec3 hitable_list::random(const vec::vec3 &o) const
{
	int index = int(rand_float() * list_size);
	return list[index]->random(o);
}

bool hitable_list::hit(const ray &ray, float t_min, float t_max, hit_record &rec) const
{
	bool hit_anything = false;
	double closest_so_far = t_max;
	for (int i = 0; i < list_size; i++)
	{
		if (list[i]->hit(ray, t_min, closest_so_far, rec)) // sphere or moving sphere instances, call its overridden virtual method
		{
			hit_anything = true;
			closest_so_far = rec.t;  // last hit time
		}
	}
	return hit_anything;
}

bool hitable_list::bounding_box(float t0, float t1, aabb &bbox) const
{
	if (list_size < 1)
		return false;
	aabb temp_box;
	bool first_true = list[0]->bounding_box(t0, t1, temp_box);
	if (!first_true)
		return false;
	else
		bbox = temp_box;
	for (int i = 1; i < list_size; i++)
	{
		if (list[0]->bounding_box(t0, t1, temp_box))
		{
			bbox = surrounding_box(bbox, temp_box);
		}
		else
			return false;
	}
	return true;
}