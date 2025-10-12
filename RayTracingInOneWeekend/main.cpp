
#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "quad.h"
#include "triangle.h"

void lostaSpheres() {

    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));


    world = hittable_list(make_shared<bvh_node>(world));
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

void lessSpheresFast() {

    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    // A single glass sphere
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    // A second sphere
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-10, 1, 0), 1.0, material2));

    // A metal sphere
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(-10, 1, -5), 1.0, material3));


    world = hittable_list(make_shared<bvh_node>(world));
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400; //1200;
    cam.samples_per_pixel = 50; //500;
    cam.max_depth         = 20; //50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

void checkered_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.3, 0.0, .3), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0, -10 ,0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio = 16.0/9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = vec3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(hittable_list(globe));
}

void perlin_spheres(){
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    camera c;

    c.aspect_ratio = 16.0/9.0;
    c.image_width = 400;
    c.samples_per_pixel = 100;
    c.max_depth = 50;

    c.vfov = 20;
    c.lookfrom = point3(13, 2, 3);
    c.lookat = point3(0, 0, 0);
    c.vup = vec3(0, 1, 0);

    c.defocus_angle = 0;

    c.render(world);

}

void quadsPretty() {
    hittable_list world;


    auto left_corner_magenta   = make_shared<lambertian>(color(0.8, 0.2, 0.8));
    auto right_corner_cyan     = make_shared<lambertian>(color(0.2, 0.8, 0.9));
    auto top_corner_yellow     = make_shared<lambertian>(color(1.0, 1.0, 0.2));
    auto bottom_corner_pink    = make_shared<lambertian>(color(1.0, 0.4, 0.6));
    auto center_lime           = make_shared<lambertian>(color(0.5, 1.0, 0.2));

    world.add(make_shared<quad>(point3(-3,-3, 5), vec3(3, 3, 0), vec3(-3, 3, 3), left_corner_magenta));   // left corner
    world.add(make_shared<quad>(point3( 3,-3, 5), vec3(-3, 3, 0), vec3(3, 3, 3), right_corner_cyan));     // right corner
    world.add(make_shared<quad>(point3(-3, 3, 5), vec3(3, -3, 0), vec3(3, -3, 3), top_corner_yellow));     // top corner
    world.add(make_shared<quad>(point3(-3,-3, 5), vec3(3, 3, 0), vec3(3, -3, -3), bottom_corner_pink));    // bottom corner
    world.add(make_shared<quad>(point3(-1,-1, 4), vec3(2, 2, 0), vec3(-2, 2, 2), center_lime));           // center

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void quadsBasic(){
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void trianlgesBasic(){
    hittable_list world;

    // Materials
    auto red      = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto green    = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto blue     = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto yellow   = make_shared<lambertian>(color(1.0, 1.0, 0.2));
    auto magenta  = make_shared<lambertian>(color(1.0, 0.2, 1.0));
    auto cyan     = make_shared<lambertian>(color(0.2, 1.0, 1.0));

    // Triangles arranged in a pattern
    // Left side - two triangles forming a diamond
    world.add(make_shared<triangle>(point3(-3, 0, 0), vec3(0, 2, 0), vec3(1.5, 0, 0), red));
    world.add(make_shared<triangle>(point3(-3, 0, 0), vec3(1.5, 0, 0), vec3(0, -2, 0), green));

    // Center - large upward pointing triangle
    world.add(make_shared<triangle>(point3(-1, -2, -1), vec3(2, 0, 0), vec3(1, 3, 0), blue));

    // Right side - two triangles forming an arrow
    world.add(make_shared<triangle>(point3(2, 1, 1), vec3(0, -2, 0), vec3(1.5, -1, 0), yellow));
    world.add(make_shared<triangle>(point3(2, -1, 1), vec3(0, -2, 0), vec3(1.5, 1, 0), magenta));
    // Bottom - wide triangle
    world.add(make_shared<triangle>(point3(-2, -3, -0.5), vec3(4, 0, 0), vec3(2, 1, 0), cyan));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void trianglesPretty(){
    hittable_list world;

    // Vibrant materials
    auto red      = make_shared<lambertian>(color(1.0, 0.1, 0.2));
    auto orange   = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto yellow   = make_shared<lambertian>(color(1.0, 0.9, 0.1));
    auto green    = make_shared<lambertian>(color(0.1, 1.0, 0.3));
    auto cyan     = make_shared<lambertian>(color(0.1, 0.8, 1.0));
    auto blue     = make_shared<lambertian>(color(0.2, 0.3, 1.0));
    auto purple   = make_shared<lambertian>(color(0.6, 0.2, 1.0));
    auto magenta  = make_shared<lambertian>(color(1.0, 0.2, 0.8));
    auto pink     = make_shared<lambertian>(color(1.0, 0.4, 0.7));
    auto lime     = make_shared<lambertian>(color(0.7, 1.0, 0.2));

    // Create a kaleidoscope pattern with 8-fold symmetry
    double radius = 2.5;
    double z_depth = -2.0;
    int segments = 8;
    
    for(int i = 0; i < segments; i++) {
        double angle1 = (2.0 * 3.14159265359 * i) / segments;
        double angle2 = (2.0 * 3.14159265359 * (i + 1)) / segments;
        
        // Calculate vertices for outer triangles
        double x1 = radius * cos(angle1);
        double y1 = radius * sin(angle1);
        double x2 = radius * cos(angle2);
        double y2 = radius * sin(angle2);
        
        // Select material based on segment
        shared_ptr<material> mat1, mat2;
        switch(i % 10) {
            case 0: mat1 = red; mat2 = orange; break;
            case 1: mat1 = orange; mat2 = yellow; break;
            case 2: mat1 = yellow; mat2 = green; break;
            case 3: mat1 = green; mat2 = cyan; break;
            case 4: mat1 = cyan; mat2 = blue; break;
            case 5: mat1 = blue; mat2 = purple; break;
            case 6: mat1 = purple; mat2 = magenta; break;
            case 7: mat1 = magenta; mat2 = pink; break;
            case 8: mat1 = pink; mat2 = lime; break;
            case 9: mat1 = lime; mat2 = red; break;
        }
        
        // Outer layer triangles
        world.add(make_shared<triangle>(
            point3(0, 0, z_depth),
            vec3(x1, y1, 0),
            vec3(x2, y2, 0),
            mat1
        ));
        
        // Inner layer triangles (smaller, offset in z)
        double inner_radius = radius * 0.6;
        double xi1 = inner_radius * cos(angle1 + 0.2);
        double yi1 = inner_radius * sin(angle1 + 0.2);
        double xi2 = inner_radius * cos(angle2 + 0.2);
        double yi2 = inner_radius * sin(angle2 + 0.2);
        
        world.add(make_shared<triangle>(
            point3(0, 0, z_depth + 0.5),
            vec3(xi2, yi2, 0),
            vec3(xi1, yi1, 0),
            mat2
        ));
        
        // Center star triangles
        double center_radius = radius * 0.3;
        double xc1 = center_radius * cos(angle1);
        double yc1 = center_radius * sin(angle1);
        double xc2 = center_radius * cos(angle2);
        double yc2 = center_radius * sin(angle2);
        
        world.add(make_shared<triangle>(
            point3(0, 0, z_depth + 1.0),
            vec3(xc1, yc1, 0),
            vec3(xc2, yc2, 0),
            i % 2 == 0 ? cyan : magenta
        ));
    }

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 800;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 40;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void checkered_triangles() {
    hittable_list world;

    // Create checkered texture
    auto checker = make_shared<checker_texture>(0.32, color(.3, 0.0, .3), color(.9, .9, .9));
    auto checker_mat = make_shared<lambertian>(checker);

    // One triangle in the center-left
    world.add(make_shared<triangle>(
        point3(-2, -2, 0),
        vec3(0, 4, 0),
        vec3(3, 0, 0),
        checker_mat
    ));

    // One quad next to it on the right
    world.add(make_shared<quad>(
        point3(2, -2, 0),
        vec3(3, 0, 0),
        vec3(0, 4, 0),
        checker_mat
    ));

    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 50;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main(){
    //lessSpheresFast();
    switch(10) {
        case 1: lessSpheresFast(); break;
        case 2: checkered_spheres(); break;
        case 3: lostaSpheres(); break;
        case 4: earth(); break;
        case 5: perlin_spheres(); break;
        case 6: quadsPretty(); break;
        case 7: quadsBasic(); break;
        case 8: trianlgesBasic(); break;
        case 9: trianglesPretty(); break;
        case 10: checkered_triangles(); break;
    }
}