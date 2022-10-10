#pragma once
#include "hitablelist.h"
#include "onb.h"
#include "rand.h"
#include "material.h"
#include "texture.h"

#pragma region sphere
class sphere : public hitable
{
public:
    sphere()
    {
        center = vec::vec3();
        radius = 1;
        mat_ptr = NULL;
    };
    sphere(vec::vec3 center, float radius, std::shared_ptr<material> mat_ptr) : center(center), radius(radius), mat_ptr(mat_ptr){};

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, aabb &bbox) const;
    float pdf_value(const vec::vec3 &o, const vec::vec3 &v) const;
    vec::vec3 random(const vec::vec3 &o) const;
    void get_sphere_uv(float &u, float &v, const vec::vec3 &point) const;

    vec::vec3 center;
    float radius;
    std::shared_ptr<material> mat_ptr;
};
inline void sphere::get_sphere_uv(float &u, float &v, const vec::vec3 &point) const
{
    float phi = atan2(point.x(), point.z()); // atan2 [-π，π]
    float theta = acos(-point.y());
    u = (phi + M_PI) / (2 * M_PI); // x
    v = theta / M_PI;              // y
}
float sphere::pdf_value(const vec::vec3 &o, const vec::vec3 &v) const
{
    hit_record rec;
    if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
    {
        float cos_theta_max = sqrt(1 - radius * radius / (center - o).squared_length());
        float solid_angle = 2 * M_PI * (1 - cos_theta_max);
        return 1 / solid_angle;
    }
    else
        return 0;
}
vec::vec3 sphere::random(const vec::vec3 &o) const
{
    vec::vec3 direction = center - o;
    float distance_squared = direction.squared_length();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
    // return uvw.local(random_cosine_direction());
}
bool sphere::hit(const ray &ray, float t_min, float t_max, hit_record &rec) const
{
    float a = vec::dot(ray.dir, ray.dir);
    float b = vec::dot(ray.dir, ray.ori - center);
    float c = vec::dot(ray.ori - center, ray.ori - center) - radius * radius;
    float delta = b * b - a * c;
    if (delta > 0)
    {
        float temp = (-b - sqrt(delta)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.point_at_parameter(rec.t);
            rec.normal = (rec.point - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.u, rec.v, rec.normal);
            return true;
        }
        temp = (-b + sqrt(delta)) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.point_at_parameter(rec.t);
            rec.normal = (rec.point - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.u, rec.v, rec.normal);
            return true;
        }
    }
    return false;
}
bool sphere::bounding_box(float t0, float t1, aabb &bbox) const
{
    bbox = aabb(center - vec::vec3(radius), center + vec::vec3(radius));
    return true;
}
#pragma endregion

#pragma region moving_sphere
class moving_sphere : public hitable
{
public:
    moving_sphere(){};
    moving_sphere(vec::vec3 center0, vec::vec3 center1, float time0, float time1, float radius, std::shared_ptr<material> mat_ptr) : center0(center0), center1(center1), time0(time0), time1(time1), radius(radius), mat_ptr(mat_ptr){};

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, aabb &bbox) const;
    vec::vec3 center(float time) const;

    vec::vec3 center0, center1;
    float time0, time1;
    float radius;
    std::shared_ptr<material> mat_ptr;
};
bool moving_sphere::hit(const ray &ray, float t_min, float t_max, hit_record &rec) const
{
    vec::vec3 moving_center = center(ray.get_time());
    vec::vec3 oc = ray.ori - moving_center;
    float a = vec::dot(ray.dir, ray.dir);
    float b = vec::dot(oc, ray.dir);
    float c = vec::dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    auto has_resolve_or_not = [&](float temp) -> bool // pointer to lambda function
    {
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.point_at_parameter(temp);
            rec.normal = (rec.point - moving_center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        return false;
    };
    if (discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant)) / a; // first solution
        if (has_resolve_or_not(temp))
            return true;
        temp = (-b + sqrt(discriminant)) / a; // second solution
        if (has_resolve_or_not(temp))
            return true;
    }
    return false;
}
bool moving_sphere::bounding_box(float t0, float t1, aabb &bbox) const
{
    aabb bbox0 = aabb(center0 - vec::vec3(radius), center0 + vec::vec3(radius));
    aabb bbox1 = aabb(center1 - vec::vec3(radius), center1 + vec::vec3(radius)); // wrong write center0, sphere is torn
    bbox = surrounding_box(bbox0, bbox1);
    return true;
}
vec::vec3 moving_sphere::center(float time) const
{
    return center0 + (time - time0) / (time1 - time0) * (center1 - center0);
}
#pragma endregion

#pragma region rectangle
class xy_rect : public hitable
{
public:
    xy_rect(){};
    xy_rect(float x0, float y0, float x1, float y1, float k, std::shared_ptr<material> mat_ptr) : x0(x0), y0(y0), x1(x1), y1(y1), k(k), mat_ptr(mat_ptr){};

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override
    {
        float t = (k - ray.origin().z()) / ray.direction().z();
        if (t < t_max && t > t_min)
        {
            float x = ray.origin().x() + t * ray.direction().x();
            float y = ray.origin().y() + t * ray.direction().y();
            if (x > x0 && x < x1 && y > y0 && y < y1)
            {
                rec.point = ray.point_at_parameter(t);
                rec.mat_ptr = mat_ptr;
                rec.normal = vec::vec3(0, 0, 1);
                rec.u = (x - x0) / (x1 - x0);
                rec.v = (y - y0) / (y1 - y0);
                rec.t = t;
                return true;
            }
        }
        return false;
    }

    virtual bool bounding_box(float t0, float t1, aabb &bbox) const override
    {
        bbox = aabb(vec::vec3(x0, y0, k - 0.0001), vec::vec3(x1, y1, k + 0.0001));
        return true;
    }

    virtual float pdf_value(const vec::vec3 &o, const vec::vec3 &v) const override
    {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
        {
            float area = (x1 - x0) * (y1 - y0);
            float distance_squared = rec.t * rec.t * v.squared_length();
            float cosine = fabs(vec::dot(v, rec.normal) / v.length());
            return distance_squared / (cosine * area);
        }
        return 0;
    }

    virtual vec::vec3 random(const vec::vec3 &o) const override
    {
        vec::vec3 random_point = vec::vec3(x0 + rand_float() * (x1 - x0), y0 + rand_float() * (y1 - y0), k);
        return random_point - o;
    }

    float x0, y0, x1, y1, k;
    std::shared_ptr<material> mat_ptr;
};

class xz_rect : public hitable
{
public:
    xz_rect(){};
    xz_rect(float x0, float z0, float x1, float z1, float k, std::shared_ptr<material> mat_ptr) : x0(x0), z0(z0), x1(x1), z1(z1), k(k), mat_ptr(mat_ptr){};

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override
    {
        float t = (k - ray.origin().y()) / ray.direction().y();
        if (t < t_max && t > t_min)
        {
            float x = ray.origin().x() + t * ray.direction().x();
            float z = ray.origin().z() + t * ray.direction().z();
            if (x > x0 && x < x1 && z > z0 && z < z1)
            {
                rec.point = ray.point_at_parameter(t);
                rec.mat_ptr = mat_ptr;
                rec.normal = vec::vec3(0, 1, 0);
                rec.u = (x - x0) / (x1 - x0);
                rec.v = (z - z0) / (z1 - z0);
                rec.t = t;
                return true;
            }
        }
        return false;
    }

    virtual bool bounding_box(float t0, float t1, aabb &bbox) const override
    {
        bbox = aabb(vec::vec3(x0, z0, k - 0.0001), vec::vec3(x1, z1, k + 0.0001));
        return true;
    }

    virtual float pdf_value(const vec::vec3 &o, const vec::vec3 &v) const override
    {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
        {
            float area = (x1 - x0) * (z1 - z0);
            float distance_squared = rec.t * rec.t * v.squared_length();
            float cosine = fabs(vec::dot(v, rec.normal) / v.length());
            return distance_squared / (cosine * area);
        }
        return 0;
    }

    virtual vec::vec3 random(const vec::vec3 &o) const override
    {
        vec::vec3 random_point = vec::vec3(x0 + rand_float() * (x1 - x0), k, z0 + rand_float() * (z1 - z0));
        return random_point - o;
    }

    float x0, z0, x1, z1, k;
    std::shared_ptr<material> mat_ptr;
};

class yz_rect : public hitable
{
public:
    yz_rect(){};
    yz_rect(float y0, float z0, float y1, float z1, float k, std::shared_ptr<material> mat_ptr) : y0(y0), z0(z0), y1(y1), z1(z1), k(k), mat_ptr(mat_ptr){};

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override
    {
        float t = (k - ray.origin().x()) / ray.direction().x();
        if (t < t_max && t > t_min)
        {
            float y = ray.origin().y() + t * ray.direction().y();
            float z = ray.origin().z() + t * ray.direction().z();
            if (z > z0 && z < z1 && y > y0 && y < y1)
            {
                rec.point = ray.point_at_parameter(t);
                rec.mat_ptr = mat_ptr;
                rec.normal = vec::vec3(1, 0, 0);
                rec.u = (y - y0) / (y1 - y0);
                rec.v = (z - z0) / (z1 - z0);
                rec.t = t;
                return true;
            }
        }
        return false;
    }

    virtual bool bounding_box(float t0, float t1, aabb &bbox) const override
    {
        bbox = aabb(vec::vec3(y0, z0, k - 0.0001), vec::vec3(y1, z1, k + 0.0001));
        return true;
    }

    virtual float pdf_value(const vec::vec3 &o, const vec::vec3 &v) const override
    {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001, FLT_MAX, rec))
        {
            float area = (y1 - y0) * (z1 - z0);
            float distance_squared = rec.t * rec.t * v.squared_length();
            float cosine = fabs(vec::dot(v, rec.normal) / v.length());
            return distance_squared / (cosine * area);
        }
        return 0;
    }

    virtual vec::vec3 random(const vec::vec3 &o) const override
    {
        vec::vec3 random_point = vec::vec3(k, y0 + rand_float() * (y1 - y0), z0 + rand_float() * (z1 - z0));
        return random_point - o;
    }

    float y0, z0, y1, z1, k;
    std::shared_ptr<material> mat_ptr;
};
#pragma endregion

#pragma region box
class box : public hitable
{
public:
    box(){};
    box(vec::vec3 pmin, vec::vec3 pmax, std::shared_ptr<material> mat_ptr) : pmin(pmin), pmax(pmax), mat_ptr(mat_ptr)
    {
        std::shared_ptr<hitable> *list = new std::shared_ptr<hitable>[6];
        list[0].reset(new xy_rect(pmin.x(), pmin.y(), pmax.x(), pmax.y(), pmax.z(), mat_ptr));
        list[1].reset(new filp_normals(std::shared_ptr<hitable>(new xy_rect(pmin.x(), pmin.y(), pmax.x(), pmax.y(), pmin.z(), mat_ptr))));
        list[2].reset(new xz_rect(pmin.x(), pmin.z(), pmax.x(), pmax.z(), pmax.y(), mat_ptr));
        list[3].reset(new filp_normals(std::shared_ptr<hitable>(new xz_rect(pmin.x(), pmin.z(), pmax.x(), pmax.z(), pmin.y(), mat_ptr))));
        list[4].reset(new yz_rect(pmin.y(), pmin.z(), pmax.y(), pmax.z(), pmax.x(), mat_ptr));
        list[5].reset(new filp_normals(std::shared_ptr<hitable>(new yz_rect(pmin.y(), pmin.z(), pmax.y(), pmax.z(), pmin.x(), mat_ptr))));
        hit_ptr = new hitable_list(list, 6);
    }

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override
    {
        return hit_ptr->hit(ray, t_min, t_max, rec);
    }

    virtual bool bounding_box(float t0, float t1, aabb &bbox) const override
    {
        bbox = aabb(pmin, pmax);
        return true;
    }

    vec::vec3 pmin, pmax;
    std::shared_ptr<material> mat_ptr;
    hitable *hit_ptr;
};
#pragma endregion

#pragma region constant_medium
class constant_medium : public hitable
{
public:
    constant_medium(){};
    constant_medium(std::shared_ptr<hitable> boundary, float density, std::shared_ptr<texture> tex) : boundary(boundary), density(density)
    {
        phase_function.reset(new isotropic(tex));
    };

    virtual bool hit(const ray &ray, float t_min, float t_max, hit_record &rec) const override;

    virtual bool bounding_box(float t0, float t1, aabb &bbox) const override
    {
        return boundary->bounding_box(t0, t1, bbox);
    }

    std::shared_ptr<hitable> boundary;
    float density;
    std::shared_ptr<material> phase_function;
};

inline bool constant_medium::hit(const ray &ray, float t_min, float t_max, hit_record &rec) const
{
    bool db = (rand_float() < 0.00001);
    db = false;
    hit_record rec1, rec2;
    if (boundary->hit(ray, -FLT_MAX, FLT_MAX, rec1)) // record1 first enter constant_medium
    {
        if (boundary->hit(ray, rec1.t + 0.0001, FLT_MAX, rec2)) // record2 last exit constant_medium
        {
            if (db)
                std::cerr << "\n0 t1" << rec1.t << " " << rec2.t << "\n";
            if (rec1.t < t_min)
                rec1.t = t_min;
            if (rec2.t > t_max)
                rec2.t = t_max;
            if (rec1.t >= rec2.t) // constant_medium back to ray
                return false;
            if (rec1.t < 0) // inside object
                rec1.t = 0;
            float distance_inside_boundary = (rec2.t - rec1.t) * ray.direction().length();
            float hit_distance = -(1. / density) * log(rand_float());
            if (hit_distance < distance_inside_boundary)
            {
                if (db)
                    std::cerr << "hit_distance = " << hit_distance << "\n";
                rec.t = rec1.t + hit_distance / ray.direction().length();
                if (db)
                    std::cerr << "rec.t = " << rec.t << "\n";
                rec.point = ray.point_at_parameter(rec.t);
                if (db)
                    std::cerr << "rec.point = " << rec.point << "\n";
                rec.normal = vec::vec3(1, 0, 0); // arbitrary
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;
}
#pragma endregion
