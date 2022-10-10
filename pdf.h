#pragma once
#include "vec3.h"
#include "onb.h"
#include "rand.h"
#include "hitable.h"

class pdf
{
public:
    virtual float value(const vec::vec3 &direction) const = 0;
    virtual vec::vec3 generate() const = 0;
};

#pragma region sphere_pdf
class sphere_pdf : public pdf
{
public:
    sphere_pdf() {}

    virtual float value(const vec::vec3 &direction) const override
    {
        return 1./(4*M_PI);
    }

    virtual vec::vec3 generate() const override { return random_in_unit_sphere(); }
};
#pragma endregion

#pragma region cosine_pdf
class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vec::vec3 &w) { uvw.build_from_w(w); }

    virtual float value(const vec::vec3 &direction) const override
    {
        float cosine = vec::dot(vec::unit_vector(direction), uvw.w());
        if (cosine > 0)
            return cosine / M_PI;
        return 0;
    }

    virtual vec::vec3 generate() const override { return uvw.local(random_cosine_direction()); }
    onb uvw;
};
#pragma endregion

#pragma region hitable_pdf
class hitable_pdf : public pdf
{
public:
    hitable_pdf(std::shared_ptr<hitable> ptr, const vec::vec3 &origin) : ptr(ptr), origin(origin){};

    virtual float value(const vec::vec3 &direction) const override
    {
        return ptr->pdf_value(origin, direction);
    }
    virtual vec::vec3 generate() const override
    {
        return ptr->random(origin);
    }

    vec::vec3 origin;
    std::shared_ptr<hitable> ptr;
};
#pragma endregion

#pragma region mixture_pdf
class mixture_pdf : public pdf
{
public:
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1)
    {
        p[0] = p0;
        p[1] = p1;
    };

    virtual float value(const vec::vec3 &direction) const override
    {
        if(!p[0])return p[1]->value(direction);
        if(!p[1])return p[0]->value(direction);
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }
    virtual vec::vec3 generate() const override
    {
        if(!p[0])p[1]->generate();
        if(!p[1])p[0]->generate();
        if (rand_float() < 0.5)
            return p[0]->generate();
        return p[1]->generate();
    }

    std::shared_ptr<pdf> p[2];
};
#pragma endregion