#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

const double M_PI = 3.14159265359;

vec3 color(const ray& r, hitable *world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main()
{
	int nx = 200;
	int ny = 100;
	int ns = 100;

	std::ofstream output("render.ppm");
	output << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *list[5];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5))); // ground
	list[1] = new sphere(vec3(0, 1, 0), 1, new metal(vec3(0.0, 0.7, 0.7), 0.2));
	list[2] = new sphere(vec3(2, 1.3, -3), 1.3, new lambertian(vec3(0.0, 0.6, 1.0)));
	list[3] = new sphere(vec3(-2, 1.3, 3), 1.3, new lambertian(vec3(0.0, 0.4, 1.0)));
	list[4] = new sphere(vec3(1, 1, 3), 0.25, new dielectric(1.5));
	hitable *world = new hitable_list(list, 5);

	vec3 lookfrom(0, 1, 9);
	vec3 lookat(0, 1, 0);
	float dist_to_focus = 6.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + ((double)rand() / (RAND_MAX))) / float(nx);
				float v = float(j + ((double)rand() / (RAND_MAX))) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);

			output << ir << " " << ig << " " << ib << "\n";
		}
	}
}
