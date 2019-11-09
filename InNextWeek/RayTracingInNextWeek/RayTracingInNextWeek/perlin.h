#ifndef PERLINEH
#define PERLINEH

#include "vec3.h"
#include "random.h"

class perlin {
    public:
        float noise(const vec3& p) const {
            float u = p.x() - floor(p.x());
            float v = p.y() - floor(p.y());
            float w = p.z() - floor(p.z());
            // int i = fabs(floor(p.x()));
            // int j = floor(p.y());
            // int k = floor(p.z());
            int i = int(p.x()*4) & 255;
            int j = int(p.y()*4) & 255;
            int k = int(p.z()*4) & 255;
            return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
        }
        static float *ranfloat;
        static int *perm_x;
        static int *perm_y;
        static int *perm_z;
};

#endif  // !PERLINEH
