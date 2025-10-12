#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable {
    public:
        triangle(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat){
            auto n = cross(u, v);
            normal = unit_vector(n);
            D = dot(normal, Q);
            w = n / dot(n, n);

            set_bounding_box();
        }

        virtual void set_bounding_box(){
            auto bbox1 = aabb(Q, Q + u);
            auto bbox2 = aabb(Q, Q + v);
            auto bbox3 = aabb(Q + u, Q + v);

            bbox = aabb(bbox1, bbox2);
            bbox = aabb(bbox, bbox3);
        }

        aabb bounding_box() const override{
            return bbox;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            auto num = dot(normal, r.direction());

            if(std::fabs(num) < 1e-8){
                return false;
            }
            auto t = (D - dot(normal, r.origin())) / num;
            if(!ray_t.contains(t)){
                return false;
            }

            auto intersect = r.at(t);

            vec3 planar_hit_vec = intersect - Q;
            auto alpha = dot(w, cross(planar_hit_vec, v));
            auto beta = dot(w, cross(u, planar_hit_vec));

            if(!is_interior(alpha, beta, rec)){
                return false;
            }

            rec.t = t;
            rec.p = intersect;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;

        }

        virtual bool is_interior(double a, double b, hit_record& rec)const{
            if(a < 0 || b < 0 || (a + b) > 1){
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

#endif