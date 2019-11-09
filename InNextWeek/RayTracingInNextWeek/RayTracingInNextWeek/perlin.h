#ifndef PERLINEH
#define PERLINEH

#include "vec3.h"
#include "random.h"

inline float trilinear_interp(float c[2][2][2], float u, float v, float w) {
    float accum = 0;
    for (int i = 0; i < 2; ++i) 
        for (int j = 0; j < 2; ++j)
            for(int k = 0; k < 2; ++k)
                accum += (i*u + (1-i)*(1-u))*
                         (j*v + (1-j)*(1-v))* 
                         (k*w + (1-k)*(1-w))*c[i][j][k];
    return accum;
}

class perlin {
    public:
        float noise(const vec3& p) const {
            float u = p.x() - floor(p.x());
            float v = p.y() - floor(p.y());
            float w = p.z() - floor(p.z());
            u = u*u*(3-2*u);
            v = v*v*(3-2*v);
            w = w*w*(3-2*w);
            int i = floor(p.x());
            int j = floor(p.y());
            int k = floor(p.z());
            // int i = int(p.x()*4) & 255;
            // int j = int(p.y()*4) & 255;
            // int k = int(p.z()*4) & 255;
            float c[2][2][2];
            for (int di = 0; di < 2; ++di)
                for (int dj = 0; dj < 2; ++dj)
                    for (int dk = 0; dk < 2; ++dk)
                    c[di][dj][dk] = ranfloat[
                        perm_x[(i+di)&255] ^
                        perm_y[(j+dj)&255] ^
                        perm_z[(k+dk)&255]
                    ];
            return trilinear_interp(c, u, v, w);
        }

        static float *ranfloat;
        static int *perm_x;
        static int *perm_y;
        static int *perm_z;
};

#endif  // !PERLINEH
