#pragma once
#include "vec3.h"
#include "rand.h"

float trilinear_interp(float c[2][2][2], float u, float v, float w);
float perlin_interp(vec::vec3 c[2][2][2], float u, float v, float w);

class perlin
{
public:
    perlin(){

    };
    float noise(const vec::vec3 &point) const;
    float turb(const vec::vec3 &p, int depth) const;

    // static float *ranfloat;
    static vec::vec3 *ranvec;
    static int *perm_x;
    static int *perm_y;
    static int *perm_z;
};

inline float perlin::noise(const vec::vec3 &point) const
{

    float u = point.x() - floor(point.x());  // u,v,w [0,1)
    float v = point.y() - floor(point.y());
    float w = point.z() - floor(point.z());
    int i = floor(point.x());
    int j = floor(point.y());
    int k = floor(point.z());
    vec::vec3 c[2][2][2];
    for (int di = 0; di < 2; ++di)
        for (int dj = 0; dj < 2; ++dj)
            for (int dk = 0; dk < 2; ++dk)
                c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
    return perlin_interp(c, u, v, w);
}

inline float perlin::turb(const vec::vec3 &point, int depth = 7) const
{
    float accum = 0;
    vec::vec3 temp_point = point;
    float weight = 1.0;
    for (int i = 0; i < depth; ++i)
    {
        accum += weight * noise(temp_point);
        weight *= 0.5;
        temp_point *= 2;
    }
    return fabs(accum);
}

inline static float *perlin_generate_float()
{
    float *p = new float[256];
    for (int i = 0; i < 256; ++i)
        p[i] = rand_float();
    return p;
}

inline static vec::vec3 *perlin_generate_vec3()
{
    vec::vec3 *p = new vec::vec3[256];
    for (int i = 0; i < 256; ++i)
        p[i] = vec::unit_vector(vec::vec3(-1 + 2 * rand_float(), -1 + 2 * rand_float(), -1 + 2 * rand_float()));
    return p;
}

inline void permute(int *p, int n)
{
    for (int i = n - 1; i > 0; --i)
    {
        int target = int(rand_float() * (i + 1));
        std::swap(p[i], p[target]);
    }
    return;
}

inline static int *perlin_generate_perm()
{
    int *p = new int[256];
    for (int i = 0; i < 256; ++i)
        p[i] = i;
    permute(p, 256);
    return p;
}

inline float trilinear_interp(float c[2][2][2], float u, float v, float w)
{
    float accum = 0;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
                accum += (i * u + (1 - i) * (1 - u)) *
                         (j * v + (1 - j) * (1 - v)) *
                         (k * w + (1 - k) * (1 - w)) * c[i][j][k];
    return accum;
}

inline float perlin_interp(vec::vec3 c[2][2][2], float u, float v, float w)
{
    float uu = u * u * (3 - 2 * u);
    float vv = v * v * (3 - 2 * v);
    float ww = w * w * (3 - 2 * w);
    float accum = 0;
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
            {
                vec::vec3 weight_v(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) *
                         (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) * vec::dot(c[i][j][k], weight_v);
            }
    return fabs(accum);
}

// float *perlin::ranfloat = perlin_generate_float();
vec::vec3 *perlin::ranvec = perlin_generate_vec3();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();