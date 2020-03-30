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
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"

using namespace std;

vec3 ray_color(const ray& r, const vec3& background, const hitable& world, int depth)
{
    hit_record rec;
    if (world.hit(r, 0.001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        double pdf;
        vec3 albedo;

        if (depth < 50 && rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf))
        {
            return emitted + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_color(scattered, background, world, depth + 1) / pdf;
        }
        else
        {
            return emitted;
        }
    }
    else
    {
        return background;
    }
}

void cornell_box(hitable **scene, camera **cam, double aspect)
{
    int i = 0;

    hitable** list = new hitable*[8];
    material* red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material* white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material* green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material* light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));

    /*material* glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190), 90, glass);
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    *scene = new hitable_list(list, i);*/

    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    auto t0 = 0.0;
    auto t1 = 1.0;

    *cam = new camera(lookfrom, lookat, vup, vfov, aspect, aperture, dist_to_focus, t0, t1);

    *scene = new hitable_list(list, i);
}

int main()
{
    ofstream file;
    file.open("image.ppm", ios::out);

    if (file.is_open())
    {
        int nx = 500;
        int ny = 500;
        int ns = 10;
        file << "P3\n" << nx << " " << ny << "\n255\n";

        hitable* world;
        camera* cam;
        float aspect = float(ny) / float(nx);
        cornell_box(&world, &cam, aspect);

        for (int j = ny - 1; j >= 0; j--)
        {
            for (int i = 0; i < nx; i++)
            {
                vec3 col(0, 0, 0);
                for (int s = 0; s < ns; s++)
                {
                    float u = float(i + random_double()) / float(nx);
                    float v = float(j + random_double()) / float(ny);
                    ray r = cam->get_ray(u, v);
                    col += ray_color(r, vec3(0,0,0), *world, 0);
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