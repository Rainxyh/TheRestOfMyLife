#pragma once
#include "hitable.h"
#include "rand.h"
#include "memory"
class bvh_node_sp : public hitable
{
public:
	bvh_node_sp() {}
	bvh_node_sp(std::shared_ptr<hitable> *l, int n);
	bvh_node_sp(std::shared_ptr<hitable> *l, int n, float time0, float time1);
	
	virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override;
	virtual bool bounding_box(float t0, float t1, aabb &bbox) const override;

	std::shared_ptr<hitable> left, right;
	aabb bbox;
};

bool bvh_node_sp::hit(const ray &_ray, float t_min, float t_max, hit_record &rec) const
{
	if (bbox.hit(_ray, t_min, t_max))
	{
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(_ray, t_min, t_max, left_rec);
		bool hit_right = right->hit(_ray, t_min, t_max, right_rec);
		if (hit_left && hit_right)  // two aabb have intersection
		{
			if (left_rec.t < right_rec.t)  // choose the closer one
				rec = left_rec;
			else
				rec = right_rec;
			return true;
		}
		else if (hit_left)
		{
			rec = left_rec;
			return true;
		}
		else if (hit_right)
		{
			rec = right_rec;
			return true;
		}
	}
	return false;
}

bool bvh_node_sp::bounding_box(float t0, float t1, aabb &box) const
{
	box = bbox;
	return true;
}

inline void quick_sort(std::shared_ptr<hitable> *list, int l,int r){
	if(l>=r)return ;

	int pl=l,pr=r-1;
	aabb* bbox_list = (aabb*)malloc(sizeof(aabb)*(r-l));
	
	for(int i=l;i<r;i++)list[i]->bounding_box(0,1,bbox_list[i-l]);
	std::shared_ptr<hitable> head = list[l];aabb flag=bbox_list[0];
	while(pl<pr){
		while(pl<pr&&bbox_list[pl-l].min().x()<=bbox_list[pr-l].min().x())--pr;
		bbox_list[pl-l]=bbox_list[pr-l];list[pl]=list[pr];
		while(pl<pr&&bbox_list[pl-l].min().x()<=bbox_list[pr-l].min().x())++pl;
		bbox_list[pr-l]=bbox_list[pl-l];list[pr]=list[pl];
	}
	list[pl]=head;
	free(bbox_list);

	quick_sort(list, l,pl);
	quick_sort(list, pl+1,r);
}

bvh_node_sp::bvh_node_sp(std::shared_ptr<hitable> *l, int n)
{
	quick_sort(l,0,n);

	if (n == 1)
		left = right = l[0];
	else if (n == 2)
	{
		left = l[0];
		right = l[1];
	}
	else
	{
		left = std::shared_ptr<hitable>(new bvh_node_sp(l, n / 2));
		right = std::shared_ptr<hitable>(new bvh_node_sp(l + n / 2, n - n / 2));
	}
	aabb box_left, box_right;
	if (!left->bounding_box(0, 0, box_left) || !right->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node_sp constructor\n";
	bbox = surrounding_box(box_left, box_right);
}

bvh_node_sp::bvh_node_sp(std::shared_ptr<hitable> *l, int n, float time0, float time1)
{
	quick_sort(l,0,n);

	if (n == 1)
		left = right = l[0];
	else if (n == 2)
	{
		left = l[0];
		right = l[1];
	}
	else
	{
		left = std::shared_ptr<hitable>(new bvh_node_sp(l, n / 2, time0, time1));
		right = std::shared_ptr<hitable>(new bvh_node_sp(l + n / 2, n - n / 2, time0, time1));
	}
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
		std::cerr << "no bounding box in bvh_node_sp constructor\n";
	bbox = surrounding_box(box_left, box_right);
}