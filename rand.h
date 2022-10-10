#pragma once
#include <random>
#include "vec3.h"

std::random_device rd;
std::default_random_engine random_engine(rd());
std::uniform_real_distribution<float> distribution(0, 1);

float rand_float()
{
	return distribution(random_engine);
}

float square_rand_float()
{
	return rand_float() * rand_float();
}

inline vec::vec3 random_to_sphere(float radius, float distance_squared)
{
	/*
	r2 = INTEGRAL_0^theta 2*Pi*f(t)sin(t), Here​ ​p(dir)=f(t)​ is an as yet uncalculated constant​ C .
	z = cos(theta) = 1 + r2*(cos(theta_max)-1)
	x = cos(phi)*sin(theta) = cos(2*Pi*r1)*sqrt(1-z^2)
	y = sin(phi)*sin(theta) = sin(2*Pi*r1)*sqrt(1-z^2)
	*/
	float r1 = rand_float();
	float r2 = rand_float();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * M_PI * r1;
	float x = cos(phi) * sqrt(1 - z * z);
	float y = sin(phi) * sqrt(1 - z * z);
	return vec::vec3(x, y, z);
}

vec::vec3 random_cosine_direction()
{
	/*
	​p(directions) = cos(theta) / Pi . ​
	z = cos(theta) = sqrt(1-r2)
	x = cos(phi)*sin(theta) = cos(2*Pi*r1)*sqrt(1-z^2) = cos(2*Pi*r1)*sqrt(r2)
	y = sin(phi)*sin(theta) = sin(2*Pi*r1)*sqrt(1-z^2) = sin(2*Pi*r1)*sqrt(r2)
	*/
	float r1 = rand_float();
	float r2 = rand_float();
	float z = sqrt(1 - r2);
	float phi = 2 * M_PI * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);
	return vec::vec3(x, y, z);
}

vec::vec3 random_in_unit_sphere()
{
	vec::vec3 p;
	p = 2 * vec::vec3(rand_float(), rand_float(), rand_float()) - vec::vec3(1, 1, 1);
	return vec::unit_vector(p);
}

vec::vec3 random_in_unit_disk()
{
	vec::vec3 p;
	do
	{
		p = 2 * vec::vec3(rand_float(), rand_float(), 0) - vec::vec3(1, 1, 0);
	} while (p.length() >= 1.0);
	return p;
}

void MC_integration_test()
{
	/* fault to use cos_theta = sqrt(1-r) which is calculated from pdf(theta) = 2*cos(theta)*sin(theta) (p(dir) = f(theta) = cos(theta)/Pi)
	   as cos_theta  = 1-r which is calculated from pdf(theta) = sin(theta) (p(dir) = f(theta) = 1/2Pi), they are not same theta.*/
	int N = 1000000;
	float sum1, sum2, sum3;
	sum1 = sum2 = sum3 = 0;
	for (int i = 1; i <= N; i++)
	{
		float pdf1 = 1. / (2 * M_PI);
		vec::vec3 point = random_in_unit_sphere();
		float cosine_squared = point.y() * point.y();
		sum1 += cosine_squared / pdf1;

		// pdf(phi)=1/2Pi, pdf(theta)=2Pi*sin(theta)*f(theta), p(dir) = f(theta)

		float r1 = rand_float(), r2 = rand_float();
		float phi, cos_theta;

		// p(dir)=f(theta)=cos(theta)/Pi
		phi = r1 * 2 * M_PI;
		cos_theta = sqrt(1. - r2);
		float pdf2 = cos_theta / M_PI;
		sum2 += pow(cos_theta, 2) / pdf2;

		// p(dir)=f(theta)=1/2Pi
		cos_theta = 1 - r2;
		sum3 += pow(cos_theta, 2) / pdf1;
	}
	float ans1, ans2, ans3;
	ans1 = sum1 / N;
	ans2 = sum2 / N;
	ans3 = sum3 / N;
	float truth = 2. / 3. * M_PI;
	std::cout << ans1 << "  " << ans1 - truth << std::endl;
	std::cout << ans2 << "  " << ans2 - truth << std::endl;
	std::cout << ans3 << "  " << ans3 - truth << std::endl;
}