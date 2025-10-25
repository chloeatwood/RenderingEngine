#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
    public:
        quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat){
            auto n = cross(u, v);
            normal = unit_vector(n);
            D = dot(normal, Q);
            w = n / dot(n, n);

            set_bounding_box();
        }

        virtual void set_bounding_box() {
            //Compute the bounding box of all four vertices
            auto bbox_diagonal1 = aabb(Q, Q + u + v);
            auto bbox_diagonal2 = aabb(Q + u, Q + v);
            bbox = aabb(bbox_diagonal1, bbox_diagonal2);

        }

        aabb bounding_box() const override {
            return bbox;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            auto num = dot(normal, r.direction());

            //No hit if ray is parallel to the plane
            if(std::fabs(num) < 1e-8){
                return false;
            }

            //return false if the hit point parameter t is outside the ray interval
            auto t = (D - dot(normal, r.origin())) / num;
            if(!ray_t.contains(t)){
                return false;
            }

            auto intersection = r.at(t);

            vec3 planar_hit_vec = intersection - Q;
            auto alpha = dot(w , cross(planar_hit_vec, v));
            auto beta = dot(w, cross(u, planar_hit_vec));

            if(!is_interior(alpha, beta, rec)){
                return false;
            }


            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        virtual bool is_interior(double a, double b, hit_record& rec) const {
            interval unit_itnerval = interval(0, 1);

            if(!unit_itnerval.contains(a) || !unit_itnerval.contains(b)){
                return false;
            }

            rec.u = a;
            rec.v = b;
            return true;
        }

    private:
        point3 Q;
        vec3 u, v;
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;
        vec3 w;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat){
    auto sides = make_shared<hittable_list>();

    auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z())); 
    auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z())); 

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0,  max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

#endif