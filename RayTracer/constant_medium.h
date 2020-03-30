#ifndef CMEDH
#define CMEDH

#include "material.h"
#include "hitable.h"

//class constant_medium : public hitable
//{
//public:
//    constant_medium(hitable* b, float d, texture* a) : boundary(b), density(d)
//    {
//        phase_function = new isotropic(a);
//    }
//
//    virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
//    virtual bool bounding_box(float t0, float t1, aabb& box) const
//    {
//        return boundary - bounding_box(t0, t1, box);
//    }
//
//    hitable* boundary;
//    float density;
//    material* phase_function;
//};
//
//bool constant_medium::hit(const ray& r, float t0, float t1, hit_record& rec) const
//{
//    const bool enableDebug = false;
//    bool debugging = enableDebug && random_double() < 0.00001;
//
//    hit_record rec1;
//    hit_record rec2;
//
//    if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
//    {
//        if (boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2))
//        {
//            if (debugging) std::cerr << "\nt0 t1" << rec1.t << " " << rec2.t << '\n';
//
//            if (rec1.t < t0)
//            {
//                rec1.t = t0;
//            }
//
//            if (rec2.t < t1)
//            {
//                rec2.t = t1;
//            }
//
//            if (rec1.t >= rec2.t)
//            {
//                return false;
//            }
//
//            if (rec1.t < 0)
//            {
//                rec1.t = 0;
//            }
//
//            float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
//            float hit_distance = -(1 / density) * log(random_double());
//
//            if (hit_distance < distance_inside_boundary)
//            {
//                rec.t = rec1.t + hit_distance / r.direction().length();
//                rec.p = r.point_at_parameter(rec.t);
//
//                if (debugging)
//                {
//                    std::cerr << "hit_distance = " << hit_distance << '\n' << "rec.t = " << rec.t << '\n' << "rec.p = " << rec.p << '\n';
//                }
//
//                rec.normal = vec3(1, 0, 0);
//                rec.mat_ptr = phase_function;
//                return true;
//            }
//        }
//    }
//
//    return false;
//}

#endif