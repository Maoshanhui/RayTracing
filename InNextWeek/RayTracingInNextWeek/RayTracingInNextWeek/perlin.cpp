#include "perlin.h"

static vec3* perlin_generate() {
    vec3* p = new vec3[256];
    for (int i = 0; i < 256; ++i) { 
        double x_random = 2 * random_double() -1;
        double y_random = 2 * random_double() -1;
        double z_random = 2 * random_double() -1;
        p[i] = unit_vector(vec3(x_random, y_random, z_random));
    }
    return p;
}

void permute(int *p, int n) {
    for (int i = n-1; i > 0; i--) {
        int target = int(random_double()*(i+1));
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
    return;
}

static int* perlin_generate_perm() {
    int * p = new int[256];
    for (int i = 0; i < 256; i++)
        p[i] = i;
    permute(p, 256);
    return p;
}


vec3 *perlin::ranvec = perlin_generate();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();


