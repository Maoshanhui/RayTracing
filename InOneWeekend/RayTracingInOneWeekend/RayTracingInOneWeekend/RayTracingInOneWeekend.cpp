//#include "stdafx.h"
#include <iostream>
#include <fstream>

#include "vec3.h"
#include "ray.h"
#include "float.h"
#include "sphere.h"
#include "hittable_list.h"
#include "camera.h"
#include "random.h"
#include "material.h"

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
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) 
		{
			return attenuation * color(scattered, world, depth +1);
		} else 
		{
			return vec3(0, 0, 0);
		}
	}
	else 
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main()
{
	ofstream outfile;
	outfile.open("./outputImage.ppm", ios::out);

	int nx = 800;
	int ny = 400;
	int ns = 100;
	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0, 2.0, 0.0);
	vec3 origin(0.0 , 0.0, 0.0);

	hittable *list[4];
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
    hittable *world =(hittable*) new hittable_list(list,4);
	camera cam;

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