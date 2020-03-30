#include "random.h"
#include "pdf.h"
#include <iostream>
#include <fstream>
#include "hitable_list.h"
#include "sphere.h"
#include <float.h>
#include "camera.h"
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

vec3 color(const ray& r, const hitable& world, hitable* light_shape, int depth)
{
    hit_record hrec;
    if (world.hit(r, 0.001, FLT_MAX, hrec))
    {
        scatter_record srec;
        vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);

        if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec))
        {
            if (srec.is_specular)
            {
                return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1);
            }

            hitable_pdf plight(light_shape, hrec.p);
            mixture_pdf p(&plight, srec.pdf_ptr);
            ray scattered = ray(hrec.p, p.generate(), r.time());
            double pdf_val = p.value(scattered.direction());
            delete srec.pdf_ptr;
            return emitted + srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered) * color(scattered, world, light_shape, depth + 1) / pdf_val;
        }
        else
        {
            return emitted;
        }
    }
    else
    {
        return vec3(0, 0, 0);
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

    material* glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190), 90, glass);

    //list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));

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
        int ns = 100;
        file << "P3\n" << nx << " " << ny << "\n255\n";

        hitable* world;
        camera* cam;
        float aspect = float(ny) / float(nx);
        cornell_box(&world, &cam, aspect);
        hitable* light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
        hitable* glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
        hitable* a[2];
        a[0] = light_shape;
        a[1] = glass_sphere;
        hitable_list hlist(a, 2);

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
                    col += color(r, *world, &hlist, 0);
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