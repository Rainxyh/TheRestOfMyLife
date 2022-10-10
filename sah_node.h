#pragma once
#include "hitable.h"
class sah_node:public hitable{

    sah_node(){};
    sah_node(hitable **l, int n);

	virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override;
	virtual bool bounding_box(float t0, float t1, aabb &bbox) const;

    hitable *left, *right;
	aabb bbox;
};

sah_node::sah_node(hitable **l, int n){
    
}
