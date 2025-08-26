#ifndef RAY_H
#define RAY_H

//the functions ray::origin() and ray::direction() both return an immutable reference to their members

class ray {
    public:
        ray() {}

        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

        const point3& origin() const {return orig; }
        const vec3& direction() const {return dir; }

        point3 at(double t) const {
            return orig + t*dir;
        }

    private:
        point3 orig;
        vec3 dir;
};

#endif