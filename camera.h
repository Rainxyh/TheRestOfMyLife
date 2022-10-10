#pragma once
#include "ray.h"
#include "rand.h"
#include "onb.h"

class camera
{
public:
	// new: add t0 and t1
	camera(){};
	camera(vec::vec3 lookfrom, vec::vec3 lookat, vec::vec3 vup, float vfov, float aspect, float aperture, float t0, float t1);

	ray get_ray(float s, float t);

	vec::vec3 origin;
	vec::vec3 u, v, w;
	vec::vec3 lower_left_corner;
	vec::vec3 horizontal;
	vec::vec3 vertical;
	float time0, time1; // new variables
	float lens_radius;
};

camera::camera(vec::vec3 lookfrom, vec::vec3 lookat, vec::vec3 vup, float vfov, float aspect, float aperture, float t0, float t1)
{
	time0 = t0;
	time1 = t1;
	lens_radius = aperture / 2.;	 // The smaller the aperture, the greater the depth of field. The larger the aperture, the smaller the depth of field.
	float theta = vfov * M_PI / 180; // focus_dist assume 1
	float half_height = tan(theta / 2);
	float half_width = aspect * half_height;
	float focus_dist = (lookfrom - lookat).length(); // lookat point related focus_dist

	origin = lookfrom;
	w = vec::unit_vector(lookfrom - lookat);										  // map to z
	u = vec::unit_vector(vec::cross(vup, w));										  // map to x
	v = vec::cross(w, u);															  // map to y
	lower_left_corner = origin - (half_width * u + half_height * v + w) * focus_dist; // oringin-focus_dist*w=lookat, similar triangle scaling 1 to real focus_dist
	horizontal = u * 2 * half_width * focus_dist;
	vertical = v * 2 * half_height * focus_dist;
}

ray camera::get_ray(float s, float t)
{
	onb uvw;
	uvw.build_from_w(w);
	vec::vec3 rd = lens_radius * uvw.local(random_in_unit_disk()); // focus effect, focus_dist not change, the lens_radius bigger, the image_dist closer
	vec::vec3 offset = u * rd.x() + v * rd.y();					   // map offsets from 3d to 2d
	float time = time0 + rand_float() * (time1 - time0);
	vec::vec3 perturbation_origin = origin + offset;
	return ray(perturbation_origin, lower_left_corner + horizontal * s + vertical * t - perturbation_origin, time); // lookat point not move,
}