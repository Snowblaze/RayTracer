#include <iostream>
#include <fstream>
#include "vec3.h"

using namespace std;

int main()
{
    ofstream file;
    file.open("image.ppm", ios::out | ios::app | ios::trunc);

    if (file.is_open())
    {
        int nx = 200;
        int ny = 100;
        file << "P3\n" << nx << " " << ny << "\n255\n";
        for (int j = ny - 1; j >= 0; j--)
        {
            for (int i = 0; i < nx; i++)
            {
                vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
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