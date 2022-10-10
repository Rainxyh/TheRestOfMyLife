#pragma once
#include <math.h>
#include <stdlib.h>
#include <iostream>
#define FLT_MAX (float)0x3f3f3f3f
bool DE_NAN = false;
namespace vec
{
	class vec3
	{
	public:
		vec3()
		{
			e[0] = 0.0;
			e[1] = 0.0;
			e[2] = 0.0;
		}
		vec3(float x)
		{
			e[0] = x;
			e[1] = x;
			e[2] = x;
		}
		vec3(float x, float y)
		{
			e[0] = x;
			e[1] = y;
		}
		vec3(float x, float y, float z)
		{
			e[0] = x;
			e[1] = y;
			e[2] = z;
		}

		inline float x() const { return e[0]; }
		inline float y() const { return e[1]; }
		inline float z() const { return e[2]; }
		inline float r() const { return e[0]; }
		inline float g() const { return e[1]; }
		inline float b() const { return e[2]; }

		inline const vec3 &operator+() const { return *this; }
		inline const vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
		inline const vec3 &operator()(const vec3 &vec) const { return vec; }
		// inline float operator[](int i) const { return e[i]; }  // 返回右值
		inline float &operator[](int i) { return e[i]; } // 返回左值

		inline vec3 &operator+=(const vec3 &v);
		inline vec3 &operator-=(const vec3 &v);
		inline vec3 &operator*=(const vec3 &v);
		inline vec3 &operator/=(const vec3 &v);
		inline vec3 &operator+=(const float v);
		inline vec3 &operator-=(const float v);
		inline vec3 &operator*=(const float v);
		inline vec3 &operator/=(const float v);

		inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
		inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
		inline void make_unit_vector();
		inline void reset()
		{
			e[0] = 0;
			e[1] = 0;
			e[2] = 0;
		}

		float e[3];
	};

	inline std::istream &operator>>(std::istream &is, const vec3 &vec)
	{
		is >> vec.e[0] >> vec.e[1] >> vec.e[2];
		return is;
	}

	inline std::ostream &operator<<(std::ostream &os, const vec3 &vec)
	{
		os << vec.e[0] << " " << vec.e[1] << " " << vec.e[2];
		return os;
	}

	inline void vec3::make_unit_vector()
	{
		float k = 1.0f / length();
		e[0] *= k;
		e[1] *= k;
		e[2] *= k;
	}

	inline vec3 operator+(const vec3 &v1, const vec3 &v2)
	{
		return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
	}

	inline vec3 operator-(const vec3 &v1, const vec3 &v2)
	{
		return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
	}

	inline vec3 operator*(const vec3 &v1, const vec3 &v2)
	{
		return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
	}

	inline vec3 operator/(const vec3 &v1, const vec3 &v2)
	{
		float threshold = 1e-5;
		if (DE_NAN && (fabs(v2.e[0]) < threshold || fabs(v2.e[1]) < threshold || fabs(v2.e[2]) < threshold))
		{
			// std::cout << "div 0 Error!!!" << std::endl;
			return v1 * FLT_MAX;
		}
		return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
	}

	inline vec3 operator*(float f, const vec3 &vec)
	{
		return vec3(vec.e[0] * f, vec.e[1] * f, vec.e[2] * f);
	}

	inline vec3 operator/(float f, const vec3 &vec)
	{
		float threshold = 1e-5;
		if (DE_NAN && (fabs(f) < threshold))
		{
			// std::cout << "div 0 Error!!!" << std::endl;
			return vec * FLT_MAX;
		}
		return vec3(vec.e[0] / f, vec.e[1] / f, vec.e[2] / f);
	}

	inline vec3 operator*(const vec3 &vec, float f)
	{
		return vec3(vec.e[0] * f, vec.e[1] * f, vec.e[2] * f);
	}

	inline vec3 operator/(const vec3 &vec, float f)
	{
		float threshold = (float)1e-5;
		if (DE_NAN && (fabs(f) < threshold))
		{
			// std::cout << "div 0 Error!!!" << std::endl;
			return vec * FLT_MAX;
		}
		return vec3(vec.e[0] / f, vec.e[1] / f, vec.e[2] / f);
	}

	inline float dot(const vec3 &v1, const vec3 &v2)
	{
		return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
	}

	inline vec3 cross(const vec3 &v1, const vec3 &v2)
	{
		return vec3(
			(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
			(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
			(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
	}

	inline vec3 &vec3::operator+=(const vec3 &v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	inline vec3 &vec3::operator-=(const vec3 &v)
	{
		e[0] -= v.e[0];
		e[1] -= v.e[1];
		e[2] -= v.e[2];
		return *this;
	}

	inline vec3 &vec3::operator*=(const vec3 &v)
	{
		e[0] *= v.e[0];
		e[1] *= v.e[1];
		e[2] *= v.e[2];
		return *this;
	}

	inline vec3 &vec3::operator/=(const vec3 &v)
	{
		e[0] /= v.e[0];
		e[1] /= v.e[1];
		e[2] /= v.e[2];
		return *this;
	}

	inline vec3 &vec3::operator+=(const float v)
	{
		e[0] += v;
		e[1] += v;
		e[2] += v;
		return *this;
	}

	inline vec3 &vec3::operator-=(const float v)
	{
		e[0] -= v;
		e[1] -= v;
		e[2] -= v;
		return *this;
	}

	inline vec3 &vec3::operator*=(const float f)
	{
		e[0] *= f;
		e[1] *= f;
		e[2] *= f;
		return *this;
	}

	inline vec3 &vec3::operator/=(const float f)
	{
		e[0] /= f;
		e[1] /= f;
		e[2] /= f;
		return *this;
	}

	inline vec3 unit_vector(const vec3 &vec)
	{
		return vec / vec.length();
	}

	inline vec3 de_nan(const vec3 &c)
	{
		vec3 temp = c;
		if (!(temp[0] == temp[0]))
			temp[0] = 0;
		if (!(temp[1] == temp[1]))
			temp[1] = 0;
		if (!(temp[2] == temp[2]))
			temp[2] = 0;
		return temp;
	}
}