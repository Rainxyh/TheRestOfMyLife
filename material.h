#pragma once
#include "ray.h"
#include "hitable.h"
#include "rand.h"
#include "pdf.h"
#include "texture.h"

struct scatter_record
{
	ray scatter_ray;
	bool perfect_specular;
	vec::vec3 attenuation; // radiance
	std::shared_ptr<pdf> pdf_ptr;
};

vec::vec3 gamma_correct(vec::vec3 col)
{
	return vec::vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
}

#pragma region material
class material
{
public:
	// hit_record is to avoid a bunch of arguments so we can stuff whatever info we want in there. You can use arguments instead;
	virtual float scattering_pdf(const ray &ray_in, const hit_record &rec, ray &scattered) const { return false; };
	virtual bool scatter(const ray &ray_in, const hit_record &hrec, scatter_record &srec) const { return false; };
	virtual bool scatter(const ray &ray_in, const hit_record &hrec, vec::vec3 &albedo, ray &scattered, float &pdf) const { return false; }; // old
	virtual const material* get_class_type(void) const = 0;
	virtual vec::vec3 emitted(const ray &ray_in, const hit_record &rec) const { return vec::vec3(0); }
};
#pragma endregion

#pragma region lambertian
class lambertian : public material
{
public:
	lambertian(){};
	lambertian(std::shared_ptr<texture> albedo) : albedo(albedo){};

	virtual float scattering_pdf(const ray &ray_in, const hit_record &rec, ray &scattered) const override
	{
		float cosine = dot(rec.normal, vec::unit_vector(scattered.direction()));
		if (cosine < 0)
			cosine = 0;
		return cosine / M_PI;
	}

	// virtual bool scatter(const ray &ray_in, const hit_record &rec, vec::vec3 &alb, ray &scattered, float &pdf) const override
	virtual bool scatter(const ray &ray_in, const hit_record &hrec, scatter_record &srec) const override
	{
		srec.perfect_specular = false;
		srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.point);
		srec.pdf_ptr.reset(new cosine_pdf(hrec.normal));
		return true;
	}

	virtual bool scatter(const ray &ray_in, const hit_record &rec, vec::vec3 &alb, ray &scattered, float &pdf) const override
	{
		vec::vec3 direction(0);
		onb uvw;
		uvw.build_from_w(rec.normal);
		direction = uvw.local(random_cosine_direction());
		scattered = ray(rec.point, vec::unit_vector(direction), ray_in.get_time());
		alb = albedo->value(rec.u, rec.v, rec.point);
		pdf = dot(uvw.w(), scattered.direction()) / M_PI;
		return true;
	}

	virtual const material *get_class_type(void) const override
	{
		return this;
	}

	std::shared_ptr<texture> albedo;
};
#pragma endregion

#pragma region metal
class metal : public material
{
public:
	metal(){};
	metal(const vec::vec3 &albedo) : albedo(albedo) { fuzz = 0.1; };
	metal(const vec::vec3 &albedo, float f) : albedo(albedo)
	{
		if (f < 1)
			fuzz = f;
		else
			fuzz = 1;
	};

	virtual bool scatter(const ray &ray_in, const hit_record &hrec, scatter_record &srec) const override
	{
		vec::vec3 reflected = reflect(vec::unit_vector(ray_in.direction()), hrec.normal);
		srec.scatter_ray = ray(hrec.point, reflected + fuzz * random_in_unit_sphere());
		srec.attenuation = albedo;
		srec.perfect_specular = true;
		srec.pdf_ptr = 0;
		return true;
	}

	virtual bool scatter(const ray &ray_in, const hit_record &rec, vec::vec3 &alb, ray &scattered, float &pdf) const override
	{
		vec::vec3 reflected = reflect(vec::unit_vector(ray_in.dir), rec.normal);
		scattered = ray(rec.point, reflected + fuzz * random_in_unit_sphere(), ray_in.get_time());
		alb = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

	virtual const material *get_class_type(void) const override
	{
		return this;
	}

	vec::vec3 albedo;
	float fuzz;
};
#pragma endregion

inline float schlick(float cosine, float eta)
{
	float r0 = (1.0f - eta) / (1.0f + eta);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * (float)pow((1 - cosine), 5);
}
#pragma region dielectric
class dielectric : public material
{
public:
	dielectric(float eta) : eta(eta){}; // eta > 1, normal is from Optically Dense to Optically Rare Medium

	virtual bool scatter(const ray &r_in, const hit_record &hrec, scatter_record &srec) const
	{
		srec.perfect_specular = true;
		srec.pdf_ptr = 0;
		srec.attenuation = vec::vec3(1.0, 1.0, 1.0);
		vec::vec3 outward_normal;
		vec::vec3 reflected = reflect(r_in.direction(), hrec.normal);
		vec::vec3 scattered;
		float ni_over_nt;
		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), hrec.normal) > 0)  // from object inner to object outter, by the way from Optically Dense to Optically Rare Medium
		{
			outward_normal = -hrec.normal;  // make incident light and normal on the same plane
			ni_over_nt = eta;
			cosine = eta * dot(r_in.direction(), hrec.normal) / r_in.direction().length();
		}
		else
		{
			outward_normal = hrec.normal;
			ni_over_nt = 1.0 / eta;
			cosine = -dot(r_in.direction(), hrec.normal) / r_in.direction().length();
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, scattered)) // if meet refracte condition, refracted could be refracte or reflect
		{
			reflect_prob = schlick(cosine, eta); // eta and 1/eta get same result
		}
		else reflect_prob=1;
		if (rand_float() < reflect_prob) // reflect must be reflect
		{
			srec.scatter_ray = ray(hrec.point, reflected);
		}
		else  // but refract could be reflect, and if full-reflect happen, refracted must be reflect
		{
			srec.scatter_ray = ray(hrec.point, scattered);
		}
		return true;
	}

	virtual const material *get_class_type(void) const override
	{
		return this;
	}

	float eta;
};
#pragma endregion

#pragma region isotropic
class isotropic : public material
{
public:
	isotropic(){};
	isotropic(std::shared_ptr<texture> albedo) : albedo(albedo){};

	virtual bool scatter(const ray &ray_in, const hit_record &hrec, scatter_record &srec) const override{
		srec.scatter_ray = ray(hrec.point, random_in_unit_sphere(), ray_in.get_time());
		srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.point);
		srec.perfect_specular = false;
		srec.pdf_ptr.reset(new sphere_pdf());
		return true;
	}

	virtual const material *get_class_type(void) const override
	{
		return this;
	}

	std::shared_ptr<texture> albedo;
};
#pragma endregion

#pragma region diffuse_light
class diffuse_light : public material
{
public:
	diffuse_light(){};
	diffuse_light(std::shared_ptr<texture> emit) : emit(emit){};
	virtual float scattering_pdf(const ray &ray_in, const hit_record &rec, ray &scattered) const override
	{
		return 1;
	}
	virtual bool scatter(const ray &ray_in, const hit_record &rec, vec::vec3 &albedo, ray &scattered, float &pdf) const override
	{
		return false;
	}
	virtual const material *get_class_type(void) const override
	{
		return this;
	}

	virtual vec::vec3 emitted(const ray &ray_in, const hit_record &rec) const override
	{
		if (vec::dot(rec.normal, ray_in.direction()) < 0.0)
			return emit->value(rec.u, rec.v, rec.point);
		return vec::vec3(0);
	}

	std::shared_ptr<texture> emit;
};
#pragma endregion