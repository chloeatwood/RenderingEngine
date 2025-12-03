

#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "quad.h"
#include "triangle.h"
#include "volume.h"
#include "mesh_loader.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

GLFWwindow* g_window = nullptr;
GLuint g_tex = 0;
bool g_use_opengl = true;
int g_image_width = 0;
int g_image_height = 0;

/*
* Taken from Ray Tracing in One Weekend. Shows lots of spheres
*/
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
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    if (g_use_opengl && g_window) {
        cam.render_opengl(world, g_window, g_tex);
    } else {
        cam.render(world);  // Original PNG output
    }
}

/*
* Used for testing the different materials of a sphere
*/
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
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    std::cout << "g_use_opengl: " << g_use_opengl << std::endl;
    std::cout << "g_window: " << g_window << std::endl;
    
    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Used to test the checkered texture.
*/
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
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Used to check loading png file onto a sphere.
*/
void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    hittable_list world; 
    world.add(globe);    

    camera cam;

    cam.aspect_ratio = 16.0/9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = vec3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }

}

/*
* Used to test the perlin noise texture in the spheres
*/
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
    c.background = color(0.70, 0.80, 1.00);

    c.vfov = 20;
    c.lookfrom = point3(13, 2, 3);
    c.lookat = point3(0, 0, 0);
    c.vup = vec3(0, 1, 0);

    c.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        c.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        c.render(world);
    }

}

/*
* Used to test rendering a bunch of quads in a cool design
*/
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
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Used to test rendering a simpler quad design
*/
void quadsBasic(){
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));


    auto purpleTexture = make_shared<image_texture>("purple.jpg");
    auto purpleText = make_shared<lambertian>(purpleTexture);

    // Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), purpleText));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), purpleText));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), purpleText));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), purpleText));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), purpleText));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Used to test simple rendering of trianlges
*/
void trianlgesBasic(){
    hittable_list world;

    // Materials
    auto red      = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto green    = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto blue     = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto yellow   = make_shared<lambertian>(color(1.0, 1.0, 0.2));
    auto magenta  = make_shared<lambertian>(color(1.0, 0.2, 1.0));
    auto cyan     = make_shared<lambertian>(color(0.2, 1.0, 1.0));

    world.add(make_shared<triangle>(point3(-3, 0, 0), vec3(0, 2, 0), vec3(1.5, 0, 0), red));
    world.add(make_shared<triangle>(point3(-3, 0, 0), vec3(1.5, 0, 0), vec3(0, -2, 0), green));

    world.add(make_shared<triangle>(point3(-1, -2, -1), vec3(2, 0, 0), vec3(1, 3, 0), blue));

    world.add(make_shared<triangle>(point3(2, 1, 1), vec3(0, -2, 0), vec3(1.5, -1, 0), yellow));
    world.add(make_shared<triangle>(point3(2, -1, 1), vec3(0, -2, 0), vec3(1.5, 1, 0), magenta));

    world.add(make_shared<triangle>(point3(-2, -3, -0.5), vec3(4, 0, 0), vec3(2, 1, 0), cyan));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Used to test triangles in a much cooler looking way
*/
void trianglesPretty(){
    hittable_list world;

    // materials
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

    // Create a kaleidoscope pattern
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
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 40;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Testing the checkered material on trianlges and quads
*/
void checkered_triangles() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.3, 0.0, .3), color(.9, .9, .9));
    auto checker_mat = make_shared<lambertian>(checker);

    //triangle
    world.add(make_shared<triangle>(point3(-2, -2, 0), vec3(0, 4, 0), vec3(3, 0, 0), checker_mat));

    // quad
    world.add(make_shared<quad>(point3(2, -2, 0), vec3(3, 0, 0), vec3(0, 4, 0), checker_mat));

    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 50;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Used to test lighting. Simple test
*/
void simple_light() {
    hittable_list world;

    auto pertxt = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertxt)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertxt)));

    auto diff = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(0, 7, 0), 2, diff));
    world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), diff));

    camera c;

    c.aspect_ratio = 16.0 / 9.0;
    c.image_width = 400;
    c.samples_per_pixel = 100;
    c.max_depth = 50;
    c.background = color(0, 0, 0);

    c.vfov = 20;
    c.lookfrom = point3(26, 3, 6);
    c.lookat = point3(0, 2, 0);
    c.vup = vec3(0, 1, 0);

    c.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        c.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        c.render(world);
    }
}

/*
* Cornell box. Mostly taken from Ray Tracing in One Weekend as well. But changed some colors and stuff
*/
void cornell_box(){
    hittable_list w;

    auto purple = make_shared<lambertian>(color(.65, .05, .65));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto lime_green = make_shared<lambertian>(color(.45, .73, .12));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    w.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), lime_green));
    w.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), purple));
    w.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    w.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    w.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    w.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    w.add(box1);

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), lime_green);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    w.add(box2);

    camera c;

    c.aspect_ratio = 1.0;
    c.image_width = 600;
    c.samples_per_pixel = 400;
    c.max_depth = 50;
    c.background = color(0, 0, 0);

    c.vfov = 40;
    c.lookfrom = point3(278, 278, -800);
    c.lookat = point3(278, 278, 0);
    c.vup = vec3(0, 1, 0);

    c.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        c.render_opengl(w, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        c.render(w);
    }

}

/*
* I do not remember the difference between this one and the previous one
*/
void cornell_smoke(){
    hittable_list world;

    auto purple = make_shared<lambertian>(color(.65, .05, .65));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto lime_green = make_shared<lambertian>(color(.45, .73, .12));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), lime_green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), purple));
    world.add(make_shared<quad>(point3(113,554,127), vec3(330,0,0), vec3(0,0,305), light));
    world.add(make_shared<quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    world.add(make_shared<volume>(box1, 0.01, color(0,0,0)));
    world.add(make_shared<volume>(box2, 0.01, color(1,1,1)));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Also taken from Ray Tracing in One Weekend. The final scene rendering image at the end of the book
*/
void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123,554,147), vec3(300,0,0), vec3(0,0,265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<volume>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(boundary, .0001, color(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-100,270,395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Testing out volumes in an attemptingly cool looking way
*/
void volume_showcase() {
    hittable_list world;

    auto light_material = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(color(0.1, 0.1, 0.15))));
    
    auto boundary1 = make_shared<sphere>(point3(-2, 2, 0), 1.5, make_shared<dielectric>(1.5));
    world.add(boundary1);
    world.add(make_shared<volume>(boundary1, 0.5, color(0.8, 0.2, 0.9)));
    
    auto boundary2 = make_shared<sphere>(point3(2, 2, -1), 1.2, make_shared<dielectric>(1.5));
    world.add(boundary2);
    world.add(make_shared<volume>(boundary2, 0.8, color(0.4, 1.0, 0.2)));
    
    auto boundary3 = make_shared<sphere>(point3(0, 3.5, 2), 1.0, make_shared<dielectric>(1.5));
    world.add(boundary3);
    world.add(make_shared<volume>(boundary3, 1.5, color(0.1, 0.9, 1.0)));
    
    auto boundary4 = make_shared<sphere>(point3(-1, 1, 3), 0.8, make_shared<dielectric>(1.5));
    world.add(boundary4);
    world.add(make_shared<volume>(boundary4, 1.2, color(1.0, 0.5, 0.1)));
    
    auto boundary5 = make_shared<sphere>(point3(1.5, 1.2, 2), 0.6, make_shared<dielectric>(1.5));
    world.add(boundary5);
    world.add(make_shared<volume>(boundary5, 2.0, color(1.0, 0.3, 0.7)));
    
    shared_ptr<hittable> smoke_box = box(point3(-1, 0, -2), point3(0.5, 1.8, -0.5), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    smoke_box = make_shared<rotate_y>(smoke_box, 25);
    world.add(make_shared<volume>(smoke_box, 0.3, color(0.6, 0.1, 0.8)));
    
    world.add(make_shared<sphere>(point3(-2, 2, 0), 0.3, make_shared<diffuse_light>(color(10, 5, 12))));
    world.add(make_shared<sphere>(point3(2, 2, -1), 0.3, make_shared<diffuse_light>(color(6, 15, 3))));
    world.add(make_shared<sphere>(point3(0, 3.5, 2), 0.2, make_shared<diffuse_light>(color(2, 12, 15))));
    
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 100, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0001, color(0.7, 0.8, 1.0)));
    
    world.add(make_shared<sphere>(point3(3, 0.7, 1), 0.7, make_shared<metal>(color(0.9, 0.9, 1.0), 0.1)));
    world.add(make_shared<sphere>(point3(-3, 0.5, 0), 0.5, make_shared<lambertian>(color(1.0, 1.0, 1.0))));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 400;
    cam.max_depth = 50;
    cam.background = color(0.05, 0.05, 0.1);

    cam.vfov = 40;
    cam.lookfrom = point3(0, 4, 12);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* More volume testing
*/
void purple() {
    hittable_list world;

    auto ground = make_shared<lambertian>(color(0.15, 0.12, 0.2));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));
    
    auto blue_cloud1 = make_shared<sphere>(point3(-4, 3, -2), 3.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(blue_cloud1, 0.2, color(0.3, 0.4, 0.8)));
    
    auto blue_cloud2 = make_shared<sphere>(point3(5, 4, 1), 4.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(blue_cloud2, 0.15, color(0.2, 0.5, 0.9)));
    
    auto blue_cloud3 = make_shared<sphere>(point3(-2, 6, 5), 3.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(blue_cloud3, 0.25, color(0.4, 0.5, 0.95)));

    auto purple_cloud1 = make_shared<sphere>(point3(3, 2, -3), 2.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_cloud1, 0.3, color(0.6, 0.3, 0.8)));
    
    auto purple_cloud2 = make_shared<sphere>(point3(-5, 5, 2), 3.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_cloud2, 0.18, color(0.7, 0.4, 0.9)));
    
    auto purple_cloud3 = make_shared<sphere>(point3(1, 7, -1), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_cloud3, 0.22, color(0.5, 0.25, 0.7)));
    
    auto pink_cloud1 = make_shared<sphere>(point3(2, 4, 3), 2.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(pink_cloud1, 0.28, color(0.9, 0.4, 0.7)));
    
    auto pink_cloud2 = make_shared<sphere>(point3(-3, 3, -1), 2.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(pink_cloud2, 0.35, color(0.95, 0.5, 0.8)));
    
    auto cyan_cloud1 = make_shared<sphere>(point3(4, 5, 4), 2.6, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_cloud1, 0.2, color(0.3, 0.8, 0.9)));
    
    auto cyan_cloud2 = make_shared<sphere>(point3(-1, 4, -4), 2.3, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_cloud2, 0.25, color(0.2, 0.9, 0.95)));
    
    auto lavender1 = make_shared<sphere>(point3(0, 2, 0), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(lavender1, 0.4, color(0.7, 0.6, 0.95)));
    
    auto lavender2 = make_shared<sphere>(point3(-2, 8, 3), 2.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(lavender2, 0.3, color(0.75, 0.65, 0.9)));

    world.add(make_shared<sphere>(point3(-3, 6, 2), 0.3, make_shared<diffuse_light>(color(8, 6, 3))));  // warm orange
    world.add(make_shared<sphere>(point3(4, 7, -1), 0.25, make_shared<diffuse_light>(color(10, 8, 4))));  // golden
    world.add(make_shared<sphere>(point3(1, 5, 4), 0.35, make_shared<diffuse_light>(color(6, 5, 10))));  // cool purple
    world.add(make_shared<sphere>(point3(-5, 4, -2), 0.28, make_shared<diffuse_light>(color(4, 8, 10))));  // cyan
    world.add(make_shared<sphere>(point3(2, 8, 1), 0.22, make_shared<diffuse_light>(color(10, 5, 7))));  // pink
    world.add(make_shared<sphere>(point3(-1, 3, -3), 0.3, make_shared<diffuse_light>(color(7, 9, 5))));  // lime
    world.add(make_shared<sphere>(point3(5, 3, 3), 0.26, make_shared<diffuse_light>(color(5, 4, 10))));  // deep blue
    world.add(make_shared<sphere>(point3(-4, 9, 0), 0.24, make_shared<diffuse_light>(color(9, 4, 8))));  // magenta
    world.add(make_shared<sphere>(point3(3, 6, -4), 0.32, make_shared<diffuse_light>(color(8, 8, 10))));  // white-blue
    world.add(make_shared<sphere>(point3(0, 10, 2), 0.2, make_shared<diffuse_light>(color(10, 7, 3))));  // amber
    
    world.add(make_shared<sphere>(point3(-6, 12, -5), 0.15, make_shared<diffuse_light>(color(5, 5, 7))));
    world.add(make_shared<sphere>(point3(7, 11, -3), 0.18, make_shared<diffuse_light>(color(7, 4, 5))));
    world.add(make_shared<sphere>(point3(-2, 13, 4), 0.16, make_shared<diffuse_light>(color(4, 6, 8))));
    world.add(make_shared<sphere>(point3(6, 14, 1), 0.14, make_shared<diffuse_light>(color(6, 6, 4))));
    
    shared_ptr<hittable> cloud_box1 = box(point3(-7, 1, -5), point3(-2, 4, 3), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    cloud_box1 = make_shared<rotate_y>(cloud_box1, 25);
    world.add(make_shared<volume>(cloud_box1, 0.08, color(0.5, 0.4, 0.85)));
    
    shared_ptr<hittable> cloud_box2 = box(point3(2, 3, -4), point3(7, 7, 2), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    cloud_box2 = make_shared<rotate_y>(cloud_box2, -30);
    world.add(make_shared<volume>(cloud_box2, 0.1, color(0.4, 0.6, 0.9)));
    
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 200, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0001, color(0.5, 0.4, 0.7)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 400;
    cam.max_depth = 100;
    cam.background = color(0.15, 0.1, 0.25); 

    cam.vfov = 55;
    cam.lookfrom = point3(0, 3, 12);
    cam.lookat = point3(0, 5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* More volume testing
*/
void dark() {
    hittable_list world;

    auto dark_ground = make_shared<lambertian>(color(0.05, 0.05, 0.08));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, dark_ground));
    
    auto cloud1 = make_shared<sphere>(point3(-3, 4, -2), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cloud1, 0.3, color(0.15, 0.15, 0.18)));
    
    auto cloud2 = make_shared<sphere>(point3(4, 5, 0), 3.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cloud2, 0.25, color(0.1, 0.1, 0.12)));
    
    auto cloud3 = make_shared<sphere>(point3(0, 6, 3), 2.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cloud3, 0.35, color(0.08, 0.08, 0.1)));
    
    auto cloud4 = make_shared<sphere>(point3(-5, 3, 2), 2.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cloud4, 0.4, color(0.12, 0.1, 0.15)));
    
    auto cloud5 = make_shared<sphere>(point3(2, 3, -4), 2.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cloud5, 0.5, color(0.18, 0.15, 0.2)));
    
    auto wisp1 = make_shared<sphere>(point3(1, 7, 1), 1.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(wisp1, 0.15, color(0.2, 0.18, 0.22)));
    
    auto wisp2 = make_shared<sphere>(point3(-2, 8, -1), 1.3, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(wisp2, 0.2, color(0.16, 0.14, 0.18)));
    
    auto storm_core = make_shared<sphere>(point3(0, 4, 0), 1.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(storm_core, 1.5, color(0.02, 0.02, 0.03)));
    

    auto charged1 = make_shared<sphere>(point3(-1, 5, -1), 0.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(charged1, 0.6, color(0.15, 0.12, 0.25)));
    
    auto charged2 = make_shared<sphere>(point3(2, 4, 1), 0.6, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(charged2, 0.8, color(0.12, 0.15, 0.3)));
    
    world.add(make_shared<sphere>(point3(-4, 6, -5), 0.5, make_shared<diffuse_light>(color(0.8, 0.9, 1.2))));
    world.add(make_shared<sphere>(point3(5, 7, -3), 0.3, make_shared<diffuse_light>(color(0.6, 0.7, 1.0))));
    world.add(make_shared<sphere>(point3(1, 9, 2), 0.4, make_shared<diffuse_light>(color(0.5, 0.6, 0.9))));
    
    shared_ptr<hittable> cloud_layer1 = box(point3(-8, 2, -6), point3(-3, 5, 2), make_shared<lambertian>(color(0.1, 0.1, 0.1)));
    cloud_layer1 = make_shared<rotate_y>(cloud_layer1, 15);
    world.add(make_shared<volume>(cloud_layer1, 0.1, color(0.14, 0.13, 0.16)));
    
    shared_ptr<hittable> cloud_layer2 = box(point3(3, 3, -4), point3(7, 6, 3), make_shared<lambertian>(color(0.1, 0.1, 0.1)));
    cloud_layer2 = make_shared<rotate_y>(cloud_layer2, -20);
    world.add(make_shared<volume>(cloud_layer2, 0.12, color(0.11, 0.1, 0.13)));
    
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 150, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.00008, color(0.1, 0.08, 0.15)));
    
    world.add(make_shared<sphere>(point3(-10, 15, -20), 0.2, make_shared<diffuse_light>(color(0.3, 0.3, 0.4))));
    world.add(make_shared<sphere>(point3(12, 18, -18), 0.15, make_shared<diffuse_light>(color(0.25, 0.25, 0.35))));
    world.add(make_shared<sphere>(point3(-8, 20, -15), 0.18, make_shared<diffuse_light>(color(0.28, 0.28, 0.38))));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.background = color(0.01, 0.01, 0.02);  // Nearly black

    cam.vfov = 50;
    cam.lookfrom = point3(0, 2, 15);
    cam.lookat = point3(0, 5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* More volume testing
*/
void colorful() {
    hittable_list world;

    auto forest_floor = make_shared<lambertian>(color(0.1, 0.15, 0.08));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, forest_floor));
    
    auto bright_cyan = make_shared<diffuse_light>(color(12, 15, 18));
    auto bright_magenta = make_shared<diffuse_light>(color(18, 8, 15));
    auto bright_lime = make_shared<diffuse_light>(color(15, 20, 8));
    auto bright_purple = make_shared<diffuse_light>(color(15, 8, 20));
    auto bright_orange = make_shared<diffuse_light>(color(20, 12, 5));
    
    
    world.add(make_shared<sphere>(point3(0, 3, 0), 0.6, bright_cyan));
    world.add(make_shared<sphere>(point3(-3, 2.5, -2), 0.5, bright_magenta));
    world.add(make_shared<sphere>(point3(3, 2.8, -1), 0.55, bright_lime));
    world.add(make_shared<sphere>(point3(-2, 4, 2), 0.45, bright_purple));
    world.add(make_shared<sphere>(point3(2, 3.5, 3), 0.5, bright_orange));
    
    
    world.add(make_shared<sphere>(point3(-1, 5, 1), 0.25, bright_cyan));
    world.add(make_shared<sphere>(point3(1.5, 4.5, -2), 0.3, bright_magenta));
    world.add(make_shared<sphere>(point3(-2.5, 3, 3), 0.28, bright_lime));
    world.add(make_shared<sphere>(point3(3.5, 5, 0), 0.22, bright_purple));
    world.add(make_shared<sphere>(point3(-3.5, 4, -1), 0.26, bright_orange));
    world.add(make_shared<sphere>(point3(0.5, 6, 2), 0.2, bright_cyan));
    world.add(make_shared<sphere>(point3(-1.5, 6.5, -1), 0.24, bright_magenta));
    
    auto cyan_mist = make_shared<sphere>(point3(-2, 2, 0), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_mist, 0.3, color(0.2, 0.8, 1.0)));
    
    auto purple_mist = make_shared<sphere>(point3(2, 2.5, -2), 2.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_mist, 0.25, color(0.7, 0.3, 0.9)));
    
    auto lime_mist = make_shared<sphere>(point3(0, 3, 3), 2.3, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(lime_mist, 0.35, color(0.6, 1.0, 0.3)));
    
    auto magenta_mist = make_shared<sphere>(point3(-3, 4, 1), 2.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(magenta_mist, 0.4, color(1.0, 0.3, 0.8)));
    
    auto orange_mist = make_shared<sphere>(point3(3, 1.5, 2), 2.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(orange_mist, 0.28, color(1.0, 0.6, 0.2)));
    
    auto crystal_purple = make_shared<lambertian>(color(0.8, 0.4, 1.0));
    auto crystal_cyan = make_shared<lambertian>(color(0.3, 0.9, 1.0));
    auto crystal_lime = make_shared<lambertian>(color(0.7, 1.0, 0.3));
    
    shared_ptr<hittable> crystal1 = box(point3(-1.5, 0, -1), point3(-0.8, 1.8, -0.3), crystal_purple);
    crystal1 = make_shared<rotate_y>(crystal1, 25);
    world.add(crystal1);
    
    shared_ptr<hittable> crystal2 = box(point3(1, 0, 0.5), point3(1.6, 2.2, 1.1), crystal_cyan);
    crystal2 = make_shared<rotate_y>(crystal2, -35);
    world.add(crystal2);
    
    shared_ptr<hittable> crystal3 = box(point3(-0.3, 0, 2), point3(0.3, 1.5, 2.5), crystal_lime);
    crystal3 = make_shared<rotate_y>(crystal3, 15);
    world.add(crystal3);
    
    auto glass = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(-2.5, 0.8, 1.5), 0.8, glass));
    world.add(make_shared<sphere>(point3(2.8, 0.6, -0.5), 0.6, glass));
    
    world.add(make_shared<sphere>(point3(1.5, 0.5, -3), 0.5, make_shared<metal>(color(0.9, 0.5, 1.0), 0.1)));
    world.add(make_shared<sphere>(point3(-3, 0.7, -2.5), 0.7, make_shared<metal>(color(0.5, 1.0, 0.9), 0.05)));
    
    shared_ptr<hittable> fog_layer1 = box(point3(-5, 0.2, -4), point3(-1, 2, 2), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_layer1 = make_shared<rotate_y>(fog_layer1, 20);
    world.add(make_shared<volume>(fog_layer1, 0.05, color(0.5, 0.7, 1.0)));
    
    shared_ptr<hittable> fog_layer2 = box(point3(1, 0.5, -3), point3(5, 3, 3), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_layer2 = make_shared<rotate_y>(fog_layer2, -25);
    world.add(make_shared<volume>(fog_layer2, 0.06, color(0.8, 0.5, 1.0)));
    
    auto ground_fog = make_shared<sphere>(point3(0, 0.3, 0), 8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(ground_fog, 0.02, color(0.4, 0.6, 0.8)));
    
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 100, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0002, color(0.4, 0.5, 0.7)));
    
    // Distant dim lights
    world.add(make_shared<sphere>(point3(-5, 2, -3), 0.15, make_shared<diffuse_light>(color(8, 10, 5))));
    world.add(make_shared<sphere>(point3(5, 3, -2), 0.12, make_shared<diffuse_light>(color(10, 5, 8))));
    world.add(make_shared<sphere>(point3(-4, 6, 2), 0.18, make_shared<diffuse_light>(color(5, 12, 10))));
    world.add(make_shared<sphere>(point3(4, 7, 1), 0.14, make_shared<diffuse_light>(color(12, 8, 5))));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 700;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.background = color(0.05, 0.08, 0.15);

    cam.vfov = 50;
    cam.lookfrom = point3(0, 2, 10);
    cam.lookat = point3(0, 2.5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.3;  // Slight depth of field 
    cam.focus_dist = 10.0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* More volume testing but with 3D trianlges. Also testing out 3D trianlges
*/
void liquidy_triangles() {
    hittable_list world;

    auto liquid_floor = make_shared<lambertian>(color(0.05, 0.1, 0.15));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, liquid_floor));
    
    auto aqua_glow = make_shared<diffuse_light>(color(8, 18, 20));
    auto teal_glow = make_shared<diffuse_light>(color(5, 20, 18));
    auto cyan_glow = make_shared<diffuse_light>(color(10, 22, 25));
    auto deep_blue_glow = make_shared<diffuse_light>(color(8, 15, 25));
    auto turquoise_glow = make_shared<diffuse_light>(color(12, 25, 22));
    
    world.add(make_shared<sphere>(point3(-2, 4, -1), 0.4, aqua_glow));
    world.add(make_shared<sphere>(point3(2.5, 3.5, 0), 0.5, teal_glow));
    world.add(make_shared<sphere>(point3(0, 5, 2), 0.35, cyan_glow));
    world.add(make_shared<sphere>(point3(-3, 3, 1), 0.45, deep_blue_glow));
    world.add(make_shared<sphere>(point3(3, 4.5, -2), 0.38, turquoise_glow));

    world.add(make_shared<sphere>(point3(-1, 6, 0), 0.2, aqua_glow));
    world.add(make_shared<sphere>(point3(1.5, 5.5, -1), 0.25, teal_glow));
    world.add(make_shared<sphere>(point3(-2.5, 4.5, 2), 0.22, cyan_glow));
    

    auto liquid_cyan = make_shared<lambertian>(color(0.2, 0.7, 0.8));
    auto liquid_teal = make_shared<lambertian>(color(0.2, 0.8, 0.7));
    auto liquid_aqua = make_shared<lambertian>(color(0.3, 0.9, 0.9));
    auto liquid_blue = make_shared<lambertian>(color(0.2, 0.6, 0.9));
    
    shared_ptr<hittable> shard1 = tetrahedron(point3(-2, 0, -1), point3(-1, 2.5, 0), liquid_cyan);
    shard1 = make_shared<rotate_y>(shard1, 15);
    world.add(shard1);
    
    shared_ptr<hittable> shard2 = tetrahedron(point3(1.5, 0, -0.5), point3(2.5, 2.8, 0.5), liquid_teal);
    shard2 = make_shared<rotate_y>(shard2, -25);
    world.add(shard2);
    
    shared_ptr<hittable> shard3 = tetrahedron(point3(-0.5, 0, 1.5), point3(0.5, 3, 2.5), liquid_aqua);
    shard3 = make_shared<rotate_y>(shard3, 35);
    world.add(shard3);
    
    shared_ptr<hittable> shard4 = tetrahedron(point3(-3, 0.5, 0.5), point3(-2, 2.2, 1.5), liquid_blue);
    shard4 = make_shared<rotate_y>(shard4, -40);
    world.add(shard4);
    
    shared_ptr<hittable> shard5 = tetrahedron(point3(2.5, 0, 1), point3(3.5, 2.5, 2), liquid_cyan);
    shard5 = make_shared<rotate_y>(shard5, 50);
    world.add(shard5);
    

    auto glass = make_shared<dielectric>(1.33); 
    world.add(make_shared<sphere>(point3(-2, 0.6, 1), 0.6, glass));
    world.add(make_shared<sphere>(point3(2, 0.5, -1.5), 0.5, glass));
    world.add(make_shared<sphere>(point3(0, 0.4, -2), 0.4, glass));
    
    world.add(make_shared<sphere>(point3(-3.5, 0.5, -1), 0.5, make_shared<metal>(color(0.7, 0.9, 1.0), 0.02)));
    world.add(make_shared<sphere>(point3(3.5, 0.6, 1), 0.6, make_shared<metal>(color(0.6, 0.95, 0.95), 0.05)));
    
    // mist
    auto cyan_mist = make_shared<sphere>(point3(-1.5, 2, 0), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_mist, 0.4, color(0.2, 0.7, 0.9)));
    
    auto teal_mist = make_shared<sphere>(point3(2, 2.5, -1), 2.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(teal_mist, 0.35, color(0.3, 0.9, 0.8)));
    
    auto aqua_mist = make_shared<sphere>(point3(-1, 3.5, 2), 2.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(aqua_mist, 0.38, color(0.4, 0.95, 0.95)));
    
    // fog?
    shared_ptr<hittable> fog_flow1 = box(point3(-4, 0.3, -3), point3(-0.5, 2.5, 1), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_flow1 = make_shared<rotate_y>(fog_flow1, 30);
    world.add(make_shared<volume>(fog_flow1, 0.08, color(0.3, 0.8, 0.9)));
    
    shared_ptr<hittable> fog_flow2 = box(point3(0.5, 0.5, -2), point3(4, 3, 2), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_flow2 = make_shared<rotate_y>(fog_flow2, -35);
    world.add(make_shared<volume>(fog_flow2, 0.07, color(0.4, 0.9, 0.85)));
    
    // mist?
    auto surface_mist = make_shared<sphere>(point3(0, 0.2, 0), 6, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(surface_mist, 0.03, color(0.4, 0.8, 0.9)));
    
    // water?
    auto water_atmosphere = make_shared<sphere>(point3(0, 0, 0), 80, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(water_atmosphere, 0.0005, color(0.3, 0.7, 0.9)));
    
    // glow
    world.add(make_shared<sphere>(point3(-5, 3, -2), 0.15, make_shared<diffuse_light>(color(6, 15, 18))));
    world.add(make_shared<sphere>(point3(5, 4, -1), 0.18, make_shared<diffuse_light>(color(8, 18, 20))));
    world.add(make_shared<sphere>(point3(-4, 5, 2), 0.12, make_shared<diffuse_light>(color(10, 20, 18))));
    world.add(make_shared<sphere>(point3(4, 6, 1), 0.16, make_shared<diffuse_light>(color(7, 22, 22))));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 700;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.background = color(0.02, 0.08, 0.12);   //blue

    cam.vfov = 45;
    cam.lookfrom = point3(0, 2.5, 9);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.4; 
    cam.focus_dist = 9.0;

    if (g_use_opengl && g_window) {
        cam.render_opengl(world, g_window, g_tex);
    } else {
        cam.render(world);
    }
}

/*
* Testing out 3D trianlges
*/
void triangle_test() {
    hittable_list world;

    // Floor
    auto floor_mat = make_shared<lambertian>(color(0.2, 0.2, 0.2));


    auto purpleTexture = make_shared<image_texture>("purpleTexture.jpg");
    auto purpleText = make_shared<lambertian>(purpleTexture);


    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, floor_mat));
    
    // Three 3D trianlges with different materials
    auto cyan = make_shared<lambertian>(color(0.2, 0.7, 0.8));
    auto glass = make_shared<dielectric>(1.5);
    auto shiny_metal = make_shared<metal>(color(0.4, 0.8, 0.9), 0.1);
    
    shared_ptr<hittable> tet1 = tetrahedron(point3(-2, 0, -1), point3(0, 2.5, 1), purpleText);
    tet1 = make_shared<rotate_y>(tet1, 40);
    world.add(tet1);
    
    shared_ptr<hittable> tet2 = tetrahedron(point3(0, 0, -1), point3(2, 2.5, 1), purpleText);
    tet2 = make_shared<rotate_y>(tet2, -50);
    world.add(tet2);
    
    shared_ptr<hittable> tet3 = tetrahedron(point3(-1, 1.5, 0), point3(1, 4, 2), purpleText);
    tet3 = make_shared<rotate_y>(tet3, 70);
    world.add(tet3);

    shared_ptr<hittable> tet4 = tetrahedron(point3(1, 2, 1), point3(3, 2.5, 15), purpleText);
    tet4 = make_shared<rotate_y>(tet4, 100);
    world.add(tet4);
    
    // Light
    auto light = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(3, 6, 2), 2, light));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 700;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = color(0.05, 0.05, 0.1);

    cam.vfov = 50;
    cam.lookfrom = point3(0, 2.5, 7);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.focus_dist = 7.0;

    if (g_use_opengl && g_window) {
        cam.render_opengl(world, g_window, g_tex);
    } else {
        cam.render(world);
    }
}

/*
* Testing out mesh rendering with a very simple shape
*/
void meshBasic(){
    hittable_list world;

    // Materials
    auto mesh_material = make_shared<lambertian>(color(0.8, 0.3, 0.3));
    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    // Load the mesh
    std::cout << "Loading cube mesh..." << std::endl;
    auto mesh = load_obj_mesh("meshes/cube.obj", mesh_material, point3(0, 0, 0), 2.0);
    world.add(mesh);
    std::cout << "Mesh added to world" << std::endl;

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;  
    cam.image_width       = 400;
    cam.samples_per_pixel = 10;  
    cam.max_depth         = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 40;
    cam.lookfrom = point3(13, 2, 3); 
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Testing out mesh loading with an obj taken from a website. Also used to test normal interpolation
*/
void mesh(){
    hittable_list world;

    // Materials
    auto mesh_material = make_shared<lambertian>(color(0.128, 0, 0.128));
    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    // Load the mesh
    std::cout << "Loading bat mesh..." << std::endl;
    auto mesh = load_obj_mesh("meshes/bat.obj", mesh_material, point3(0, 0, 0), 2.5);
    world.add(mesh);
    std::cout << "Mesh added to world" << std::endl;

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;  
    cam.image_width       = 400;
    cam.samples_per_pixel = 50;  
    cam.max_depth         = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov     = 30;
    cam.lookfrom = point3(13, 7, 13);  
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

/*
* Turning cornell_box into cornell_bat with a bunch of bats
*/
void cornell_bat(){
    hittable_list w;

    auto purple = make_shared<lambertian>(color(.65, .05, .65));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto lime_green = make_shared<lambertian>(color(.45, .73, .12));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    auto dark_purple = make_shared<lambertian>(color(0.128, 0, 0.128));


    auto albedo = color::random(0.5, 1);
    auto fuzz = random_double(0, 0.5);
    auto metal_mat = make_shared<metal>(albedo, fuzz);
    auto glass = make_shared<dielectric>(1.5);

    w.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), lime_green));
    w.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), purple));
    w.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    w.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    w.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    w.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    // First bat - purple, upper position
    std::cout << "Loading bat mesh 1..." << std::endl;
    auto bat1 = load_obj_mesh("meshes/bat.obj", dark_purple, point3(165, 330, 165), 100.0);
    w.add(bat1);
    std::cout << "Bat 1 added to world" << std::endl;

    // Second bat - metal, lower position
    std::cout << "Loading bat mesh 2..." << std::endl;
    auto bat2 = load_obj_mesh("meshes/bat.obj", metal_mat, point3(450, 100, 180), 100.0);
    w.add(bat2);
    std::cout << "Bat 2 added to world" << std::endl;


    // Third bat - glass, lower position
    std::cout << "Loading bat mesh 2..." << std::endl;
    auto bat3 = load_obj_mesh("meshes/bat.obj", glass, point3(278, 278, 278), 80.0);
    w.add(bat3);
    std::cout << "Bat 3 added to world" << std::endl;


    camera c;

    c.aspect_ratio = 1.0;
    c.image_width = 600;
    c.samples_per_pixel = 3000;
    c.max_depth = 50;
    c.background = color(0, 0, 0);

    c.vfov = 40;
    c.lookfrom = point3(278, 278, -800);
    c.lookat = point3(278, 278, 0);
    c.vup = vec3(0, 1, 0);

    c.defocus_angle = 0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        c.render_opengl(w, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        c.render(w);
    }
}

/*
* Testing out HDR and comparing with other cornell_box renders to see the differences
*/
void cornell_box_HDR() {
    hittable_list w;

    auto red   = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    
    auto light = make_shared<diffuse_light>(color(900, 800, 700));

    auto glass = make_shared<dielectric>(1.5);
    auto metal_mat = make_shared<metal>(color(0.95, 0.95, 0.98), 0.01);

    w.add(make_shared<quad>(point3(555,0,0),     vec3(0,555,0), vec3(0,0,555), green));
    w.add(make_shared<quad>(point3(0,0,0),       vec3(0,555,0), vec3(0,0,555), red));
    w.add(make_shared<quad>(point3(0,0,0),       vec3(555,0,0), vec3(0,0,555), white));
    w.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    w.add(make_shared<quad>(point3(0,0,555),     vec3(555,0,0), vec3(0,555,0), white));

    w.add(make_shared<quad>(point3(213,554,227), vec3(130,0,0), vec3(0,0,105), light));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), glass);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    w.add(box1);

    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), metal_mat);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    w.add(box2);

    w.add(make_shared<sphere>(point3(400, 150, 200), 80, glass));

    camera cam;
    cam.aspect_ratio      = 1.0;
    cam.image_width       = 900;
    cam.samples_per_pixel = 2500;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.exposure = 0.025;

    if (g_use_opengl && g_window) {
        cam.render_opengl(w, g_window, g_tex);
    } else {
        cam.render(w);
    }
}

/*
* Testing cube maps
*/
void cubemap_test() {
    hittable_list world;
    

    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));
    
    auto glass = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(-2, 1, 0), 1.0, glass));
    
    auto metal_mat = make_shared<metal>(color(0.9, 0.9, 0.95), 0.1);
    world.add(make_shared<sphere>(point3(2, 1, 0), 1.0, metal_mat));
    
    auto diff = make_shared<lambertian>(color(0.8, 0.2, 0.2));
    world.add(make_shared<sphere>(point3(0, 1, 2), 1.0, diff));
    
    world = hittable_list(make_shared<bvh_node>(world));
    
    CubeMap cubemap;
    bool cubemap_loaded = false;
    
    if (cubemap.load_cubemap("./EnvironmentMaps/lake.png")) {
        std::cout << "Environment map loaded successfully!" << std::endl;
        cubemap_loaded = true;
    } else {
        std::cout << "Failed to load environment map, using fallback background" << std::endl;
    }
    
    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 1000;
    cam.max_depth = 50;
    cam.background = color(0.7, 0.8, 1.0);
    cam.exposure = 4.0;  
    
    cam.vfov = 40;
    cam.lookfrom = point3(0, 1.5, 8);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);
    
    cam.defocus_angle = 0.2;
    cam.focus_dist = 8.0;
    
    if (cubemap_loaded) {
        cam.use_cubemap = true;
        cam.cubemap = &cubemap;
    }
    
    if (g_use_opengl && g_window) {
        std::cout << "Rendering cubemap test with OpenGL" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Rendering cubemap test to PNG" << std::endl;
        cam.render(world);
    }
}

/*
* Messing around to try and find a final render image to submit
*/
void final_showcase() { //No
    hittable_list world;
    
    // Ground with checkered pattern
    auto checker = make_shared<checker_texture>(0.5, color(0.1, 0.1, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -1001, 0), 1000, make_shared<lambertian>(checker)));
    
    // Glass sphere
    world.add(make_shared<sphere>(point3(-6, 1.5, 0), 1.0, make_shared<dielectric>(1.5)));
    
    // Metal sphere
    world.add(make_shared<sphere>(point3(-3, 1.5, 0), 1.0, make_shared<metal>(color(0.7, 0.6, 0.5), 0.1)));
    
    // Diffuse sphere
    world.add(make_shared<sphere>(point3(0, 1.5, 0), 1.0, make_shared<lambertian>(color(0.8, 0.2, 0.2))));
    
    // light box
    auto light_mat = make_shared<diffuse_light>(color(10, 10, 10));
    shared_ptr<hittable> light_box = box(point3(2, 0.5, -2), point3(4, 2.5, 0), light_mat);
    world.add(light_box);
    
    // Volumetric sphere 
    auto boundary = make_shared<sphere>(point3(3, 1.5, 3), 1.5, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<volume>(boundary, 0.4, color(0.2, 0.8, 1.0)));
    
    // Perlin noise texture
    auto perlin_tex = make_shared<noise_texture>(2.0);
    world.add(make_shared<sphere>(point3(6, 1.5, 0), 1.0, make_shared<lambertian>(perlin_tex)));
    
    // Textured quad
    auto texture = make_shared<image_texture>("earthmap.jpg");
    world.add(make_shared<quad>(point3(4, 0, 3), vec3(3, 0, 0), vec3(0, 3, 0), make_shared<lambertian>(texture)));
    
    // Triangle
    world.add(make_shared<triangle>(point3(-6, 0, -3), vec3(0, 2, 0), vec3(2, 0, 0), make_shared<lambertian>(color(1.0, 1.0, 0.2))));
    
    // Quad
    world.add(make_shared<quad>(point3(-2, 0, -4), vec3(2, 0, 0), vec3(0, 2, 0), make_shared<lambertian>(color(0.2, 1.0, 0.8))));
    
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 50, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.001, color(0.9, 0.9, 1.0)));
    
    world = hittable_list(make_shared<bvh_node>(world));
    
    CubeMap cubemap;
    bool use_env = cubemap.load_cubemap("./EnvironmentMaps/satara.png");
    
    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400; 
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.15, 0.15, 0.2);
    cam.exposure = 1.5;
    
    cam.vfov = 50;
    cam.lookfrom = point3(0, 3, 12);
    cam.lookat = point3(0, 1.5, 0);
    cam.vup = vec3(0, 1, 0);
    
    cam.defocus_angle = 0.5;
    cam.focus_dist = 12.0;
    
    if (use_env) {
        cam.use_cubemap = true;
        cam.cubemap = &cubemap;
    }

    if (g_use_opengl && g_window) {
        cam.render_opengl(world, g_window, g_tex);
    } else {
        cam.render(world);
    }
}

/*
* More messing around trying to find final rendering image to submit. 
*/
void cool() { //Has lots of potential but needs some work
    hittable_list world;
    
    auto ground = make_shared<lambertian>(color(0.05, 0.05, 0.1));
    world.add(make_shared<sphere>(point3(0, -1001, 0), 1000, ground));

    auto glass_clear = make_shared<dielectric>(1.5);
    auto glass_cyan = make_shared<lambertian>(color(0.3, 0.9, 1.0));
    

    for (int i = 0; i < 4; i++) {
        double angle = (M_PI / 2.0) * i;
        double x = 4.0 * cos(angle);
        double z = 4.0 * sin(angle);

        for (int j = 0; j < 5; j++) {
            double y = 0.3 + j * 1.2;
            double radius = 0.6 - (j * 0.08);
            auto color_gradient = color(0.2 + j * 0.15, 0.5 + j * 0.1, 1.0 - j * 0.1);
            world.add(make_shared<sphere>(point3(x, y, z), radius, 
                make_shared<metal>(color_gradient, 0.05)));
        }
    }
    
 
    auto crystal_material = make_shared<dielectric>(1.6);
    

    for (int i = 0; i < 12; i++) {
        double angle = (2.0 * M_PI / 12.0) * i;
        double x = 3.5 * cos(angle);
        double z = 3.5 * sin(angle);
        world.add(make_shared<sphere>(point3(x, 3.0, z), 0.35, crystal_material));
    }
    

    for (int i = 0; i < 12; i++) {
        double angle = (2.0 * M_PI / 12.0) * i;
        double x = 3.0 * cos(angle);
        double y = 2.0 + 2.0 * sin(angle);
        world.add(make_shared<sphere>(point3(x, y, 0), 0.3, crystal_material));
    }
    

    auto core_light = make_shared<diffuse_light>(color(15, 12, 20));
    world.add(make_shared<sphere>(point3(0, 2.5, 0), 0.8, core_light));
    

    auto cyan_mist = make_shared<sphere>(point3(0, 1.5, 0), 6.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_mist, 0.15, color(0.1, 0.6, 0.9)));
 
    auto purple_mist1 = make_shared<sphere>(point3(-3, 3, -3), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_mist1, 0.25, color(0.8, 0.3, 1.0)));
    
    auto purple_mist2 = make_shared<sphere>(point3(3, 3, 3), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_mist2, 0.25, color(0.8, 0.3, 1.0)));
    
    auto light_warm = make_shared<diffuse_light>(color(18, 14, 8));
    auto light_cool = make_shared<diffuse_light>(color(8, 14, 20));

    world.add(make_shared<sphere>(point3(-5, 4, -5), 0.4, light_warm));
    world.add(make_shared<sphere>(point3(-6, 5.5, 0), 0.35, light_warm));
    world.add(make_shared<sphere>(point3(-5, 3.5, 4), 0.4, light_warm));
    

    world.add(make_shared<sphere>(point3(5, 4, 5), 0.4, light_cool));
    world.add(make_shared<sphere>(point3(6, 5.5, 0), 0.35, light_cool));
    world.add(make_shared<sphere>(point3(5, 3.5, -4), 0.4, light_cool));
    

    auto purple_metal = make_shared<metal>(color(0.9, 0.4, 1.0), 0.1);
    auto cyan_metal = make_shared<metal>(color(0.4, 0.9, 1.0), 0.1);
    
    shared_ptr<hittable> tet1 = tetrahedron(point3(-2, 5, -2), point3(-1, 6, -1), purple_metal);
    tet1 = make_shared<rotate_y>(tet1, 45);
    world.add(tet1);
    
    shared_ptr<hittable> tet2 = tetrahedron(point3(2, 5, 2), point3(1, 6, 1), cyan_metal);
    tet2 = make_shared<rotate_y>(tet2, -45);
    world.add(tet2);
    

    // BAT
    auto mesh_material = make_shared<lambertian>(color(0.8, 0.7, 1.0));  // Bright lavender
    auto mesh = load_obj_mesh("meshes/bat.obj", mesh_material, point3(0.0, 0.1, 0.5), 1.2);
    world.add(mesh);
    
    
    // image texture
    auto purple_texture = make_shared<image_texture>("purple.jpg");
    auto purple = make_shared<lambertian>(purple_texture);
    world.add(make_shared<sphere>(point3(-4.5, 0.6, 2), 0.6, purple));
    world.add(make_shared<sphere>(point3(4, 0.7, -3), 0.7, purple));
    world.add(make_shared<sphere>(point3(-3, 0.5, -3), 0.5, purple));
    
    // perlin noise
    auto perlin_tex = make_shared<noise_texture>(3.0);
    auto perlin_mat = make_shared<lambertian>(perlin_tex);
    world.add(make_shared<sphere>(point3(4.5, 0.6, 2), 0.6, perlin_mat));
    world.add(make_shared<sphere>(point3(-2.5, 0.5, -4), 0.5, perlin_mat));
    world.add(make_shared<sphere>(point3(2, 0.6, 4), 0.6, perlin_mat));
    
    // motion blur
    auto moving_material1 = make_shared<lambertian>(color(0.9, 0.3, 0.5));
    auto mb_center1_1 = point3(-5, 0.7, -1);
    auto mb_center1_2 = mb_center1_1 + vec3(1.2, 0, 0);
    world.add(make_shared<sphere>(mb_center1_1, mb_center1_2, 0.7, moving_material1));
    
    auto moving_material2 = make_shared<lambertian>(color(0.3, 0.9, 0.7));
    auto mb_center2_1 = point3(5, 0.6, 2.5);
    auto mb_center2_2 = mb_center2_1 + vec3(0, 0, -1.5);
    world.add(make_shared<sphere>(mb_center2_1, mb_center2_2, 0.6, moving_material2));
    
    auto moving_material3 = make_shared<lambertian>(color(0.8, 0.7, 0.2));
    auto mb_center3_1 = point3(-2, 0.65, 4.5);
    auto mb_center3_2 = mb_center3_1 + vec3(1.0, 0.5, 0);
    world.add(make_shared<sphere>(mb_center3_1, mb_center3_2, 0.65, moving_material3));
    

    auto floor_metal = make_shared<metal>(color(0.5, 0.6, 0.8), 0.15);
    

    for (int i = -3; i <= 3; i += 2) {
        for (int j = -3; j <= 3; j += 2) {
            world.add(make_shared<quad>(
                point3(i, 0.02, j),
                vec3(1.5, 0, 0),
                vec3(0, 0, 1.5),
                floor_metal
            ));
        }
    }
    
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 100, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0008, color(0.8, 0.7, 1.0)));
    
    world = hittable_list(make_shared<bvh_node>(world));
    
    CubeMap cubemap;
    bool use_env = cubemap.load_cubemap("./EnvironmentMaps/satara.png");
    
    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1500;  
    cam.samples_per_pixel = 10000; //On PC at home try 10000 (10 thousand)
    cam.max_depth = 60;
    cam.background = color(0.02, 0.02, 0.05);
    cam.exposure = 1.5;
    
    cam.vfov = 50;
    cam.lookfrom = point3(8, 3, 8);
    cam.lookat = point3(0, 1.5, 0);
    cam.vup = vec3(0, 1, 0);
    
    cam.defocus_angle = 0.3;
    cam.focus_dist = 11.3;
    
    if (use_env) {
        cam.use_cubemap = true;
        cam.cubemap = &cubemap;
    }
    
    // Render
    if (g_use_opengl && g_window) {
        cam.render_opengl(world, g_window, g_tex);
    } else {
        cam.render(world);
    }
}

/*
* Function that is used to select the image I want to render
*/
void summon_image(){
    //lessSpheresFast();
    switch(27) {
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
        case 11: simple_light(); break;
        case 12: cornell_box(); break;
        case 13: cornell_smoke(); break;
        case 14: final_scene(400, 3000, 4); break;
        case 15: volume_showcase(); break;
        case 16: purple(); break;
        case 17: dark(); break;
        case 18: colorful(); break;
        case 19: liquidy_triangles(); break;
        case 20: triangle_test(); break;
        case 21: meshBasic(); break;
        case 22: mesh(); break;
        case 23: cornell_bat(); break;
        case 24: cornell_box_HDR(); break;
        case 25: cubemap_test(); break;
        case 26: final_showcase(); break;
        case 27: cool(); break;
    }
}

/*
* Used for my pop-up window using openGL
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Get texture dimensions
    int tex_width, tex_height;
    glBindTexture(GL_TEXTURE_2D, g_tex);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height);
    
    if (tex_width == 0 || tex_height == 0) {
        glViewport(0, 0, width, height);
        return;
    }
    
    float img_aspect = (float)tex_width / tex_height;
    float win_aspect = (float)width / height;
    
    int vp_x, vp_y, vp_width, vp_height;
    
    if (win_aspect > img_aspect) {
        // Window is wider
        vp_height = height;
        vp_width = (int)(height * img_aspect);
        vp_x = (width - vp_width) / 2;
        vp_y = 0;
    } else {
        // Window is taller
        vp_width = width;
        vp_height = (int)(width / img_aspect);
        vp_x = 0;
        vp_y = (height - vp_height) / 2;
    }
    
    glViewport(vp_x, vp_y, vp_width, vp_height);
}

int main() {
    glfwInit();
    g_window = glfwCreateWindow(800, 450, "Ray Tracer", NULL, NULL);
    glfwMakeContextCurrent(g_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glGenTextures(1, &g_tex);
    glBindTexture(GL_TEXTURE_2D, g_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    g_use_opengl = true;
    summon_image();
    
    while (!glfwWindowShouldClose(g_window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_tex);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(-1, -1);
        glTexCoord2f(1, 1); glVertex2f( 1, -1);
        glTexCoord2f(1, 0); glVertex2f( 1,  1);
        glTexCoord2f(0, 0); glVertex2f(-1,  1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}