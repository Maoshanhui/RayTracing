#ifndef AABBH
#define AABBH

#include "hittable.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class aabb {
    public:
        aabb() {}
        aabb(const vec3& a, const vec3& b) { _min = a; _max = b;}

        vec3 min() const { return _min; }
        vec3 max() const { return _max; }

        bool hit(const ray& r, float tmin, float tmax) const;
        // {
        //     for (int a = 0; a < 3; a++) {
        //         float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
        //                         (_max[a] - r.origin()[a]) / r.direction()[a]);
        //         float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
        //                         (_max[a] - r.origin()[a]) / r.direction()[a]);
        //         tmin = ffmax(t0, tmin);
        //         tmax = ffmin(t1, tmax);
        //         if (tmax <= tmin)
        //             return false;
        //     }
        //     return true;
        // }

        vec3 _min;
        vec3 _max;
};

inline bool aabb::hit(const ray& r, float tmin, float tmax) const {
    for (int a = 0; a < 3; ++a) {
        float invD = 1.0f / r.direction()[a];
        float t0 = (min()[a] - r.origin()[a]) * invD;
        float t1 = (max()[a] - r.origin()[a]) * invD;
        if (invD < 0.0f) 
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin)
            return false;
    }
    return true;
}

#endif // !AABBH