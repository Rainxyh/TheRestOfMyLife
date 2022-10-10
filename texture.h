#pragma once
#include "vec3.h"
#include "perlin.h"

class texture
{
public:
    texture() {}
    virtual vec::vec3 value(float u, float v, const vec::vec3 &point) const = 0;
    virtual const texture *get_class_type(void) const = 0;
};

class constant_texture : public texture
{
public:
    constant_texture();
    constant_texture(vec::vec3 color) : color(color){};

    virtual vec::vec3 value(float u, float v, const vec::vec3 &point) const override
    {
        return color;
    }

    virtual const constant_texture *get_class_type(void) const override
    {
        return this;
    }

    vec::vec3 color;
};

class image_texture : public texture
{
public:
    struct tex_data_node
    {
        unsigned char * tex_data;
        int nx,ny,nn;
    };
    unsigned char *data;
    int nx, ny, nn;

    image_texture() {}
    image_texture(unsigned char *pixels, int nx, int ny, int nn) : data(pixels), nx(nx), ny(ny), nn(nn) {}
    image_texture(tex_data_node node) : data(node.tex_data), nx(node.nx), ny(node.ny), nn(node.nn) {}

    //输入u和v，输出对应图片像素的rgb值
    virtual vec::vec3 value(float u, float v, const vec::vec3 &p) const
    {
        int i = int((u)*nx); //求出像素索引
        int j = int((1 - v) * ny - 0.001f);
        if (i < 0)
            i = 0;
        if (j < 0)
            j = 0;
        if (i > nx - 1)
            i = nx - 1;
        if (j > ny - 1)
            j = ny - 1;
        float r = int(data[nn * (i + nx * j)]) / 255.0f;
        float g = int(data[nn * (i + nx * j) + 1]) / 255.0f;
        float b = int(data[nn * (i + nx * j) + 2]) / 255.0f;
        return vec::vec3(r, g, b);
    }

    virtual const image_texture *get_class_type(void) const override
    {
        return this;
    }
};

class checker_texture : public texture
{
public:
    checker_texture();
    checker_texture(texture **tex_list, float stride) : tex_list(tex_list), stride(stride){};
    virtual vec::vec3 value(float u, float v, const vec::vec3 &point) const override
    {
        float sines = sin(stride * point.x()) * sin(stride * point.z());

        if (sines < 0)
            return tex_list[0]->value(u, v, point)*rand_float();
        return tex_list[1]->value(u, v, point);
    }

    virtual const checker_texture *get_class_type(void) const override
    {
        return this;
    }

    texture **tex_list;
    float stride;
};

class noise_texture : public texture
{
public:
    noise_texture(){};
    noise_texture(float scale) : scale(scale){};
    virtual vec::vec3 value(float u, float v, const vec::vec3 &point) const override
    {
        // return vec::vec3(1) * noise.noise(scale * point);
        return vec::vec3(1) * 0.5 * (1 + sin(scale * point.z() + 10 * noise.turb(point)));
    }

    virtual const noise_texture *get_class_type(void) const override
    {
        return this;
    }

    perlin noise;
    float scale;
};