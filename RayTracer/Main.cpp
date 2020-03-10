#include <iostream>
#include <fstream>
#include "hitable_list.h"
#include "sphere.h"
#include <float.h>
#include "camera.h"
#include "random.h"
#include "material.h"
#include "moving_sphere.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "surface_texture.h"

using namespace std;

vec3 color(const ray& r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return vec3(0, 0, 0);
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

hitable* earth()
{
    int nx;
    int ny;
    int nn;
    unsigned char* tex_data = stbi_load("./assets/earthmap.jpg", &nx, &ny, &nn, 0);
    material* mat = new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0, 0, 0), 2, mat);
}

hitable* random_scene()
{
    int n = 50000;
    hitable** list = new hitable*[n + 1];
    texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -10; a < 10; a++)
    {
        for (int b = -10; b < 10; b++)
        {
            float choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - vec3(4.0, 0.2, 0.0)).length() > 0.9)
            {
                if (choose_mat < 0.8)
                {
                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * random_double(), 0.0), 0.0, 1.0, 0.2, new lambertian(new constant_texture(vec3(random_double() * random_double(), random_double() * random_double(), random_double() * random_double()))));
                }
                else if (choose_mat < 0.95)
                {
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + random_double()), 0.5 * (1 + random_double()), 0.5 * (1 + random_double())), 0.5 * random_double()));
                }
                else
                {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list, i);
}

hitable* two_spheres()
{
    texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    int n = 2;
    hitable** list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
    list[1] = new sphere(vec3(0,  10, 0), 10, new lambertian(checker));

    return new hitable_list(list, n);
}

hitable* two_perlin_spheres()
{
    texture* pertext = new noise_texture(4);
    hitable** list = new hitable* [2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new hitable_list(list, 2);
}

int main()
{
    ofstream file;
    file.open("image.ppm", ios::out);

    if (file.is_open())
    {
        int nx = 200;
        int ny = 200;
        int ns = 20;
        file << "P3\n" << nx << " " << ny << "\n255\n";

        hitable* world = earth();

        vec3 lookfrom(13, 2, 3);
        vec3 lookat(0, 0, 0);
        float dist_to_focus = 10.0;
        float aperture = 0.0;
        camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

        for (int j = ny - 1; j >= 0; j--)
        {
            for (int i = 0; i < nx; i++)
            {
                vec3 col(0, 0, 0);
                for (int s = 0; s < ns; s++)
                {
                    float u = float(i + random_double()) / float(nx);
                    float v = float(j + random_double()) / float(ny);
                    ray r = cam.get_ray(u, v);
                    col += color(r, world, 0);
                }
                col /= float(ns);
                col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
                int ir = int(255.99 * col[0]);
                int ig = int(255.99 * col[1]);
                int ib = int(255.99 * col[2]);

                file << ir << " " << ig << " " << ib << "\n";
            }
        }

        file.close();
    }

    return 0;
}