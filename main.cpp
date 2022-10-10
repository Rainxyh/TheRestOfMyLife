#include <fstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory>
#include "scene.cpp"
#include "pdf.h"

clock_t START_TIME, END_TIME;

vec::vec3 get_color(const ray &ray_, std::shared_ptr<hitable> world, std::shared_ptr<hitable> light_space, int depth)
{
	hit_record hrec;
	scatter_record srec;
	vec::vec3 emmited;
	std::shared_ptr<pdf> hp, cp, p;
	ray scattered;
	float pdf_val;
	// superclass call implemented superclass virtual methods
	if (world->hit(ray_, 0.001, FLT_MAX, hrec)) // hitable_list or bvh_node instance, calls its overridden virtual method, get the closest hit object's hrec, which determines how the color of the pixel is calculated
	{
		emmited = hrec.mat_ptr->emitted(ray_, hrec);			   // get emmited color
		if (depth < 50 && hrec.mat_ptr->scatter(ray_, hrec, srec)) // scatter (reflect and refract) happen
		{
			if (typeid(lambertian) == typeid(*hrec.mat_ptr->get_class_type()))
			{
				std::shared_ptr<lambertian> temp_ptr = std::static_pointer_cast<lambertian>(hrec.mat_ptr);
				// lambertian *temp_ori_ptr = static_cast<typename std::shared_ptr<lambertian>::element_type *>(hrec.mat_ptr.get());
				// std::shared_ptr<lambertian> temp_ptr = std::shared_ptr<lambertian>(hrec.mat_ptr, temp_ori_ptr);

				if (typeid(image_texture) == typeid(*temp_ptr->albedo->get_class_type()))
					return srec.attenuation;
			}

			if (srec.perfect_specular)
				return srec.attenuation * get_color(srec.scatter_ray, world, light_space, depth + 1); // perfect reflection of metal, and reflection or refraction of dielectric

			cp = srec.pdf_ptr; // cosine pdf, sample
			if (!light_space)
			{
				scattered = ray(hrec.point, cp->generate(), ray_.get_time());
				return emmited + srec.attenuation * get_color(scattered, world, light_space, depth + 1);
			}
			hp.reset(new hitable_pdf(light_space, hrec.point)); // hitable pdf, sample certain object
			p.reset(new mixture_pdf(hp, cp));					// mixture pdf

			scattered = ray(hrec.point, p->generate(), ray_.get_time());
			pdf_val = p->value(scattered.direction());
			return emmited + srec.attenuation * hrec.mat_ptr->scattering_pdf(ray_, hrec, scattered) * get_color(scattered, world, light_space, depth + 1) / pdf_val;
		} // hit light source
		else
			return emmited;
	}
	else
	{
		// skylight
		// vec::vec3 unit_direction = vec::unit_vector(ray_.direction());
		// float t = 0.5 * (unit_direction.y() + 1.0); // faded with y
		// return ((1.0 - t) * vec::vec3(1, 1, 1) + t * vec::vec3(0.5, 0.7, 1.0)) * 0.3;

		// darkness
		return vec::vec3(0);
	}
}

void showProgress(int num, int sum)
{
	// std::cout << "\n";
	std::cout << "\r";
	std::cout << "running: " << (sum - num) * 100 / sum << "%";
}

hitable *fun(camera &cam, std::string &fig_name, hitable *(*func_ptr)(camera &cam, std::string &fig_name))
{
	return func_ptr(cam, fig_name);
}

int main(int argc, char *argv[])
{
	START_TIME = clock();

	std::string fig_name;
	camera camera;

	int light_count;
	std::shared_ptr<hitable> light_list[100];
	// hitable *world = TheNextWeek(camera,fig_name);
	std::shared_ptr<hitable> world(scene::solar(camera, fig_name, light_list, light_count));
	std::shared_ptr<hitable> hlist(new hitable_list(light_list, light_count));

	std::string file_path = "./output_fig/";
	if (0 != access(file_path.c_str(), 0))
	{
		if (mkdir(file_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO))
		{
			std::cerr << "make dir error" << std::endl;
		}
	}
	std::ofstream outfile;
	outfile.open(file_path + fig_name + "_HD.ppm");
	// int nx = 400;
	// int ny = 300;
	// int ns = 100;
	int nx = 4096;
	int ny = 3072;
	int ns = 1000;
	outfile << "P3\n"
			<< nx << " " << ny << "\n255" << std::endl;

	int ir, ig, ib;
	float u, v;
	ray ray;
	vec::vec3 color;
	for (int j = ny; j >= 0; --j)
	{
		showProgress(j, ny);
		for (int i = 0; i < nx; ++i)
		{
			color.reset();
			for (int s = 0; s < ns; ++s) // every pixel random generate ray
			{
				u = ((float)i + rand_float()) / nx, v = ((float)j + rand_float()) / ny;
				ray = camera.get_ray(u, v); // ray's time initial wrong to 0, lead to

				color += 1.0 / (float)ns * de_nan(get_color(ray, world, hlist, 1));
			}
			color = gamma_correct(color);
			ir = int(255.99f * color.r());
			ig = int(255.99f * color.g());
			ib = int(255.99f * color.b());
			outfile << ir << " " << ig << " " << ib << std::endl;
		}
	}

	END_TIME = clock();
	double delta_time = (double)(END_TIME - START_TIME) / CLOCKS_PER_SEC;
	std::cout << "Total time " << delta_time << " s" << std::endl;
	return 0;
}