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

using namespace std;

vec3 random_in_unit_sphere() 
{
	vec3 p;
	do 
	{
		p = 2.0 * vec3(random_double(), random_double(), random_double()) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}

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

vec3 color(const ray& r, hittable *world) 
{
	hit_record rec;
	if(world->hit(r, 0.001, FLT_MAX, rec)) 
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * color(ray(rec.p, target - rec.p), world); // 递归地去计算反射光线，0.5衰减 
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

	hittable *list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
    hittable *world =(hittable*) new hittable_list(list,2);
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
				col += color(r, world);
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