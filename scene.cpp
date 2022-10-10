#pragma once
#include <string>
#include "hitablelist.h"
#include "camera.h"
#include "bvh_node.h"
#include "file.h"
#include "material.h"
#include "texture.h"
#include "geometry.h"
#include "bvh_node_sp.h"
#include "map"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace scene
{
	vec::vec3 vup = vec::vec3(0, 1, 0);
	float vfov = 40;
	float aspect = float(4) / float(3);
	float aperture = 0;
	float dist_to_focus = 10;
	float time0 = 0.0, time1 = 1.0;

	hitable *cornell_box(camera &cam, std::string &fig_name, std::shared_ptr<hitable> *light_list, int &light_count)
	{
		fig_name = "cornell_box";

		vec::vec3 lookfrom = vec::vec3(278, 278, -800);
		vec::vec3 lookat = vec::vec3(278, 278, 0);
		dist_to_focus = (lookfrom - lookat).length();
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		light_count = 0;
		std::shared_ptr<hitable> light_space(new xz_rect(213, 227, 343, 332, 554, 0));
		std::shared_ptr<hitable> glass_sphere(new sphere(vec::vec3(190, 90, 190), 50, 0));
		light_list[light_count++] = std::move(light_space);
		light_list[light_count++] = std::move(glass_sphere);

		int i = 0;
		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[8];
		std::shared_ptr<material> red(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.65, 0.05, 0.05)))));
		std::shared_ptr<material> white(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.73, 0.73, 0.73)))));
		std::shared_ptr<material> green(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.12, 0.45, 0.15)))));
		std::shared_ptr<material> blue(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.12, 0.15, 0.75)))));
		std::shared_ptr<material> yellow(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.93, 0.93, 0.15)))));
		std::shared_ptr<material> gray(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.3, 0.3, 0.3)))));
		std::shared_ptr<material> light(new diffuse_light(std::shared_ptr<texture>(new constant_texture(vec::vec3(15, 15, 15)))));
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new yz_rect(0, 0, 555, 555, 555, green))));	  // left
		list[i++].reset(new yz_rect(0, 0, 555, 555, 0, red));													  // right
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new xz_rect(213, 227, 343, 332, 554, light)))); // light
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new xz_rect(0, 0, 555, 555, 555, yellow))));	  // top
		list[i++].reset(new xz_rect(0, 0, 555, 555, 0, blue));													  // bottom
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new xy_rect(0, 0, 555, 555, 555, white))));	  // background
		std::shared_ptr<material> glass(new dielectric(1.5));
		list[i++].reset(new sphere(vec::vec3(190, 90, 190), 90, glass));
		// list[i++].reset(new translate(std::shared_ptr<hitable>(new rotate_y(std::shared_ptr<hitable>(new box(vec::vec3(0, 0, 0), vec::vec3(165, 330, 165), white)), 15)), vec::vec3(265, 0, 295)));
		std::shared_ptr<hitable> smoke_box(new translate(std::shared_ptr<hitable>(new rotate_y(std::shared_ptr<hitable>(new box(vec::vec3(0, 0, 0), vec::vec3(165, 330, 165), white)), 15)), vec::vec3(265, 0, 295)));
		list[i++].reset(new constant_medium(smoke_box, 0.01, std::shared_ptr<texture>(new constant_texture(vec::vec3(1, 1, 1)))));

		return new hitable_list(list, i);
		// return new bvh_node_sp(list, i, 0, 1);
	}

	hitable *cornell_room(camera &cam, std::string &fig_name)
	{
		fig_name = "cornell_room";

		vec::vec3 lookfrom = vec::vec3(278, 278, -800);
		vec::vec3 lookat = vec::vec3(278, 278, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[6];
		int i = 0;
		std::shared_ptr<material> red(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.65, 0.05, 0.05)))));
		std::shared_ptr<material> white(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.73, 0.73, 0.73)))));
		std::shared_ptr<material> green(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.12, 0.45, 0.15)))));
		std::shared_ptr<material> blue(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.12, 0.15, 0.75)))));
		std::shared_ptr<material> yellow(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.93, 0.93, 0.15)))));
		std::shared_ptr<material> gray(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.3, 0.3, 0.3)))));
		std::shared_ptr<material> light(new diffuse_light(std::shared_ptr<texture>(new constant_texture(vec::vec3(15, 15, 15)))));
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new yz_rect(0, 0, 555, 555, 555, green))));	  // left
		list[i++].reset(new yz_rect(0, 0, 555, 555, 0, red));													  // right
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new xz_rect(213, 227, 343, 332, 554, light)))); // top light
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new xz_rect(0, 0, 555, 555, 555, yellow))));	  // top
		list[i++].reset(new xz_rect(0, 0, 555, 555, 0, blue));													  // bottom
		list[i++].reset(new filp_normals(std::shared_ptr<hitable>(new xy_rect(0, 0, 555, 555, 555, white))));	  // background
		return new hitable_list(list, i);
	}

	hitable *simple_light(camera &cam, std::string &fig_name)
	{
		fig_name = "simple_light";

		vec::vec3 lookfrom = vec::vec3(13, 2, 3);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		std::shared_ptr<texture> pertext(new noise_texture(4));
		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[4];
		list[0].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<material>(new lambertian(pertext))));
		list[1].reset(new sphere(vec::vec3(0, 2, 0), 2, std::shared_ptr<material>(new lambertian(pertext))));
		list[2].reset(new sphere(vec::vec3(0, 7, 0), 2, std::shared_ptr<material>(new diffuse_light(std::shared_ptr<texture>(new constant_texture(vec::vec3(4, 4, 4)))))));
		list[3].reset(new xy_rect(3, 1, 5, 3, -2, std::shared_ptr<material>(new diffuse_light(std::shared_ptr<texture>(new constant_texture(vec::vec3(4, 4, 4)))))));
		return new hitable_list(list, 4);
	}

	hitable *earth(camera &cam, std::string &fig_name)
	{
		fig_name = "earth";

		vfov = 90;
		vec::vec3 lookfrom = vec::vec3(0, 0, 3);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		int nx, ny, nn;
		unsigned char *tex_data = stbi_load("./solar_texture/2k_earth.jpg", &nx, &ny, &nn, 0);
		std::shared_ptr<material> mat(new lambertian(std::shared_ptr<image_texture>(new image_texture(tex_data, nx, ny, nn))));
		return new rotate_y(std::shared_ptr<hitable>(new sphere(vec::vec3(0, 0, 0), 2, mat)), 0);
	}

	hitable *two_perlin_spheres(camera &cam, std::string &fig_name)
	{
		fig_name = "two_perlin_spheres";

		vec::vec3 lookfrom = vec::vec3(13, 2, 3);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[2];
		float scale = 5;
		std::shared_ptr<texture> p_texture(new noise_texture(scale));
		list[0].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<lambertian>(new lambertian(p_texture))));
		list[1].reset(new sphere(vec::vec3(0, -2, 0), 2, std::shared_ptr<material>(new lambertian(p_texture))));
		hitable *world = new hitable_list(list, 2);
		return world; // return point to hitable
	}

	hitable *two_spheres(camera &cam, std::string &fig_name)
	{
		fig_name = "two_spheres";

		vec::vec3 lookfrom = vec::vec3(13, 2, 3);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[2];
		// std::shared_ptr<texture> c_texture = std::shared_ptr<texture>(new checker_texture(new constant_texture(vec::vec3(0.2, 0.3, 0.1)), new constant_texture(vec::vec3(0.9, 0.9, 0.9)), 5));
		// list[0].reset(new sphere(vec::vec3(0, -10, 0), 10, std::shared_ptr<material>(new lambertian(c_texture))));
		// list[1].reset(new sphere(vec::vec3(0, 10, 0), 10, std::shared_ptr<material>(new lambertian(c_texture))));
		hitable *world = new hitable_list(list, 2); // two objects bvh slow than direct compute
		// hitable *world = new bvh_node(list, 2, 0.0, 1.0);
		return world; // return point to hitable
	}

	hitable *InOneWeekend_sp(camera &cam, std::string &fig_name)
	{
		fig_name = "InOneWeekend_sp";

		vec::vec3 lookfrom = vec::vec3(13, 2, 3);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		int n = 500;
		int count = 0, bvh_count = 0;
		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[n + 1];
		std::shared_ptr<hitable> *bvh_list = new std::shared_ptr<hitable>[n + 1];

		int nx, ny, nn;
		unsigned char *tex_data = stbi_load("./solar_texture/2k_stars_milky_way.jpg", &nx, &ny, &nn, 0);
		std::shared_ptr<material> mat(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data, nx, ny, nn))));
		// list[count++].reset(new sphere(vec::vec3(0, -1000, 0), 1000, mat));
		list[count++].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.5, 0.5, 0.5)))))));
		// list[count++].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new checker_texture(new constant_texture(vec::vec3(0.2, 0.3, 0.1)), new constant_texture(vec::vec3(0.9, 0.9, 0.9)), 5))))));
		for (int a = -10; a < 10; a++)
		{
			for (int b = -10; b < 10; b++)
			{
				float choose_mat = rand_float();
				vec::vec3 center(a + 0.9 * rand_float(), 0.2, b + 0.9 * rand_float());
				if ((center - vec::vec3(4, 0.2, 0)).length() > 0.9)
				{
					if (choose_mat < 0.8)
					{ // diffuse
						vec::vec3 moving_center = center + vec::vec3(0, 0.3 * rand_float(), 0);
						bvh_list[bvh_count++].reset(new moving_sphere(center, moving_center, 0.0, 1.0, 0.2, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(square_rand_float(), square_rand_float(), square_rand_float())))))));
					}
					else if (choose_mat < 0.95)
					{ // metal
						bvh_list[bvh_count++].reset(new sphere(center, 0.2, std::shared_ptr<material>(new metal(0.5 * vec::vec3(1 + rand_float(), 1 + rand_float(), 1 + rand_float()), 0.5 * rand_float()))));
					}
					else
					{ // glass
						bvh_list[bvh_count++].reset(new sphere(center, 0.2, std::shared_ptr<material>(new dielectric(1.5))));
					}
				}
			}
		}
		tex_data = stbi_load("./solar_texture/2k_earth.jpg", &nx, &ny, &nn, 0);
		mat.reset(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data, nx, ny, nn))));
		list[count++].reset(new sphere(vec::vec3(-4, 1, 0), 1.0, std::shared_ptr<material>(mat)));
		list[count++].reset(new sphere(vec::vec3(0, 1, 0), 1.0, std::shared_ptr<material>(new dielectric(1.5))));
		list[count++].reset(new sphere(vec::vec3(0, 1, 0), -0.95, std::shared_ptr<material>(new dielectric(1.5))));
		list[count++].reset(new sphere(vec::vec3(4, 1, 0), 1.0, std::shared_ptr<material>(new metal(vec::vec3(0.7, 0.6, 0.5), 0.0))));
		list[count++].reset(new bvh_node_sp(bvh_list, bvh_count, 0, 1));

		// superclass pointer points to child class reference
		// hitable *world = new hitable_list(list, count); // hitable_list still is a hitable, but list contains sphere
		hitable *world = new bvh_node_sp(list, count, 0.0, 1.0);
		return world; // return point to hitable
	}

	hitable *solar(camera &cam, std::string &fig_name, std::shared_ptr<hitable> *light_list, int &light_count)
	{
		fig_name = "solar";
		std::string tex_file_dir = "./solar_texture";

		vec::vec3 lookfrom = vec::vec3(9, 1, 4);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		int n = 500;
		int count = 0, bvh_count = 0;
		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[n + 1];
		hitable **bvh_list = new hitable *[n + 1];

		int nx, ny, nn;
		std::vector<std::string> files;
		GetFileName(tex_file_dir, files);

		std::map<std::string, int> name_index_map;
		image_texture::tex_data_node *tex_data_list = new image_texture::tex_data_node[files.size()];
		for (int i = 0; i < files.size(); i++)
		{
			tex_data_list[i].tex_data = stbi_load(files[i].c_str(), &tex_data_list[i].nx, &tex_data_list[i].ny, &tex_data_list[i].nn, 0);
			name_index_map[files[i]] = i;
		}
		std::shared_ptr<material> img_mat(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data_list[name_index_map[tex_file_dir + "/2k_stars.jpg"]]))));
		list[count++].reset(new sphere(vec::vec3(0, 0, 0), -30, img_mat)); // skylight
		list[count++].reset(new sphere(vec::vec3(-4, 3, 2), 3.0, std::shared_ptr<material>(new metal(vec::vec3(0.7, 0.6, 0.5), 0.0))));
		list[count++].reset(new sphere(vec::vec3(0, 2, 0), 2.0, std::shared_ptr<material>(new dielectric(1.5))));
		list[count++].reset(new sphere(vec::vec3(0, 2, 0), -1.95, std::shared_ptr<material>(new dielectric(1.5))));
		img_mat.reset(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data_list[name_index_map[tex_file_dir + "/2k_moon.jpg"]]))));
		list[count++].reset(new sphere(vec::vec3(0, 0.65, 0), 0.6, img_mat));
		img_mat.reset(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data_list[name_index_map[tex_file_dir + "/2k_earth_daymap.jpg"]]))));
		list[count++].reset(new sphere(vec::vec3(4, 1, -0.5), 1, img_mat)); // skylight
		// list[count++].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(0.5, 0.5, 0.5)))))));
		texture **tex_list = new texture *[2];
		tex_list[0] = new constant_texture(vec::vec3(10));
		tex_list[1] = new constant_texture(vec::vec3(0.1));
		list[count++].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new checker_texture(tex_list, 5))))));
		for (int a = -10; a < 10; a++)
		{
			for (int b = -10; b < 10; b++)
			{
				float choose_mat = rand_float(), rand_move = rand_float();
				vec::vec3 center(a + 0.9 * rand_float(), 0.2, b + 0.9 * rand_float());
				if ((center - vec::vec3(-4, 3, 2)).length() < 3.2 || (center - vec::vec3(0, 2, 0)).length() < 2.2 || (center - vec::vec3(4, 1, -0.5)).length() < 1.2)
					continue;
				if (choose_mat < 0.4)
				{ // diffuse
					if (rand_move < 0.1)
					{
						vec::vec3 moving_center = center + vec::vec3(0, 0.3 * rand_float(), 0);
						bvh_list[bvh_count++] = new moving_sphere(center, moving_center, 0.0, 1.0, 0.2, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(square_rand_float(), square_rand_float(), square_rand_float()))))));
					}
					else
					{
						img_mat.reset(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data_list[int(files.size() * rand_float())]))));
						bvh_list[bvh_count++] = new sphere(center, 0.2, img_mat);
						// bvh_list[bvh_count++] = new sphere(center, 0.2, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new constant_texture(vec::vec3(square_rand_float(), square_rand_float(), square_rand_float()))))));
					}
				}
				else if (choose_mat < 0.6)
				{ // metal
					if (rand_move < 0.1)
					{
						vec::vec3 moving_center = center + vec::vec3(0, 0.2 * rand_float(), 0);
						bvh_list[bvh_count++] = new moving_sphere(center, moving_center, 0.0, 1.0, 0.2, std::shared_ptr<material>(new metal(0.5 * vec::vec3(1 + rand_float(), 1 + rand_float(), 1 + rand_float()), 0.5 * rand_float())));
					}
					else
					{
						bvh_list[bvh_count++] = new sphere(center, 0.2, std::shared_ptr<material>(new metal(0.5 * vec::vec3(1 + rand_float(), 1 + rand_float(), 1 + rand_float()), 0.5 * rand_float())));
					}
				}
				else
				{ // glass
					if (rand_move < 0.1)
					{
						vec::vec3 moving_center = center + vec::vec3(0, 0.1 * rand_float(), 0);
						bvh_list[bvh_count++] = new moving_sphere(center, moving_center, 0.0, 1.0, 0.2, std::shared_ptr<material>(new dielectric(1.5)));
					}
					else
					{
						bvh_list[bvh_count++] = new sphere(center, 0.2, std::shared_ptr<material>(new dielectric(1.5)));
						if (choose_mat > 0.8)
							bvh_list[bvh_count++] = new sphere(center, -0.18, std::shared_ptr<material>(new dielectric(1.5)));
					}
				}
			}
		}
		list[count++].reset(new bvh_node(bvh_list, bvh_count, 0, 1));

		std::shared_ptr<material> strong_light_mat(new diffuse_light(std::shared_ptr<texture>(new constant_texture(vec::vec3(10)))));
		std::shared_ptr<material> weak_light_mat(new diffuse_light(std::shared_ptr<texture>(new constant_texture(vec::vec3(5)))));
		std::shared_ptr<hitable> *sphere_list = new std::shared_ptr<hitable>[3];
		sphere_list[0].reset(new sphere(vec::vec3(-8, 10, 0), 1.0, strong_light_mat));
		sphere_list[1].reset(new sphere(vec::vec3(0, 15, 0), 1.0, strong_light_mat));
		sphere_list[2].reset(new sphere(vec::vec3(15, 2, 3), 1.0, weak_light_mat));
		list[count++] = sphere_list[0];
		list[count++] = sphere_list[1];
		list[count++] = sphere_list[2];
		light_count = 0;
		light_list[light_count++] = sphere_list[0];
		light_list[light_count++] = sphere_list[1];
		light_list[light_count++] = sphere_list[2]; 

		// superclass pointer points to child class reference
		hitable *world = new hitable_list(list, count); // hitable_list still is a hitable, but list contains sphere
		return world;									// return point to hitable
	}

	// big ball left, no problem,otherwise double layer glass refract wrong (no refract just reflect)
	// which is bvh's BUG
	hitable *glass_ball(camera &cam, std::string &fig_name)
	{
		fig_name = "glass_ball";

		vec::vec3 lookfrom = vec::vec3(0, 3, 10);
		vec::vec3 lookat = vec::vec3(0, 0, 0);
		cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, time0, time1);

		std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[5];
		int count = 0;

		// list[count++].reset(new sphere(vec::vec3(0, -1000, 0), 1000, std::shared_ptr<material>(new lambertian(std::shared_ptr<texture>(new checker_texture(new constant_texture(vec::vec3(0.1, 0.1, 0.1)), new constant_texture(vec::vec3(0.9, 0.9, 0.9)), 5))))));
		int nx, ny, nn;
		unsigned char *tex_data = stbi_load("./solar_texture/2k_earth.jpg", &nx, &ny, &nn, 0);
		std::shared_ptr<material> mat(new lambertian(std::shared_ptr<texture>(new image_texture(tex_data, nx, ny, nn))));
		list[count++].reset(new sphere(vec::vec3(-4, 2, 0), 1.0, mat));
		list[count++].reset(new sphere(vec::vec3(0, 2, 0), 2.0, std::shared_ptr<material>(new dielectric(1.5))));
		list[count++].reset(new sphere(vec::vec3(0, 2, 0), -1.95, std::shared_ptr<material>(new dielectric(1.5))));

		hitable *world = new hitable_list(list, count); // hitable_list still is a hitable, but list contains sphere
		// hitable *world = new bvh_node(list, count, 0.0, 1.0);
		return world; // return point to hitable
	}
}
