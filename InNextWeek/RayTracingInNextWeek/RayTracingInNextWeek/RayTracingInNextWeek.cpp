//#include "stdafx.h"
#include <iostream>
#include <fstream>

#include "vec3.h"
#include "ray.h"
#include "float.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "hittable_list.h"
#include "camera.h"
#include "random.h"
#include "material.h"
#include "texture.h"
#include "aarect.h"
#include "box.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

float hit_sphere(const vec3& center, float radius, const ray&r) 
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b*b - 4*a*c;
	if (discriminant < 0) 
	{
		return -1.0;
	}
	else 
	{
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
	return (discriminant > 0); 
}

vec3 color(const ray& r, hittable *world, int depth) 
{
	hit_record rec;
	if(world->hit(r, 0.001, FLT_MAX, rec)) 
	{
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) 
		{
			return emitted + attenuation * color(scattered, world, depth +1);
		} else 
		{
			return emitted;
		}
	}
	else
	return vec3(0,0,0); 
	// {
	// 	vec3 unit_direction = unit_vector(r.direction());
	// 	float t = 0.5 * (unit_direction.y() + 1.0);
	// 	return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	// }
}

hittable *random_scene() {
    int n = 500;
    hittable **list = new hittable*[n+1];
	texture* checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)), 
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new moving_sphere(
						center, 
						center + vec3(0, 0.5*random_double(), 0), 
						0.0, 1.0, 0.2,
                        new lambertian(new constant_texture(vec3(
											random_double()*random_double(),
                                            random_double()*random_double(),
                                            random_double()*random_double()))
                        )
                    );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + random_double()),
                                           0.5*(1 + random_double()),
                                           0.5*(1 + random_double())),
                                      0.5*random_double()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return (hittable*) new hittable_list(list,i);
}

hittable* two_spheres() {
	texture *checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)), 
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	int n = 50;
	hittable **list = new hittable*[n+1];
	list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
	list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));
	return (hittable*) new hittable_list(list, 2);
}

hittable* two_perlin_spheres() {
	texture *pertext = new noise_texture(10);
	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	return (hittable*) new hittable_list(list, 2);
}

hittable* earth() {
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex_data, nx, ny));
	return new sphere(vec3(0, 0, 0), 2, mat);
}

hittable* simple_light() {
	texture* pertext = new noise_texture(4);
	hittable **list = new hittable*[4];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	list[2] = new sphere(vec3(0, 7, 0), 2, 
		new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2,
		new diffuse_light(new constant_texture(vec3(4,4,4))));
	return (hittable*) new hittable_list(list, 4);
}

hittable* cornell_box() {
	hittable **list = new hittable*[7];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	// box 
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
 	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	return (hittable*) new hittable_list(list, i);
}

int main()
{
	ofstream outfile;
	outfile.open("./outputImage.ppm", ios::out);

	int nx = 400;
	int ny = 200;
	int ns = 100;
	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0, 2.0, 0.0);
	vec3 origin(0.0 , 0.0, 0.0);

	// hittable *list[5];
	// list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
	// list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	// list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
	// list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	// list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
    // hittable *world =(hittable*) new hittable_list(list,5);

	// vec3 lookfrom(3,3,2);
	// vec3 lookat(0,0,-1);
	// float dist_to_focus = (lookfrom-lookat).length();
	// float aperture = 2.0;

	// camera cam(lookfrom, lookat, vec3(0,1,0), 20,
    //         float(nx)/float(ny), aperture, dist_to_focus);

	// hittable *list[2];
	// float R = cos(M_PI / 4);
	// list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
	// list[1] = new sphere(vec3(R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
	// hittable *world = (hittable*) new hittable_list(list, 2);
	// camera cam(90, float(nx)/float(ny));

	//hittable* world = random_scene();
	//hittable* world = two_spheres();
	//hittable* world = two_perlin_spheres();
	//hittable* world = earth();
	//hittable* world = simple_light();
	hittable* world = cornell_box();

	// vec3 lookfrom(13, 2, 3);
    // vec3 lookat(0, 0, 0);
	// float dist_to_focus = 10.0f;
    // float aperture = 0.0;
    // camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture,
	// dist_to_focus, 0.0, 1.0);

	// vec3 lookfrom(5, 8, 10);
	// vec3 lookat(0, 0, 0);
	// float dist_to_focus = 10.0;
	// float aperture = 0.0;
	// camera cam(lookfrom, lookat, vec3(0, 1, 0), 40, float(nx) / float(ny), 0.0, dist_to_focus, 0., 1.);

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278,278,0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny),
    	aperture, dist_to_focus, 0.0, 1.0);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; ++i)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; ++s) {
				float u = float(i + random_double()) / float(nx);
				float v = float(j + random_double()) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}
    return 0;
}