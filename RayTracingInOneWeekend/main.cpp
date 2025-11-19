
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

void quadsBasic(){
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));


    auto purpleTexture = make_shared<image_texture>("purpleTexture.jpg");
    auto purpleText = make_shared<lambertian>(purpleTexture);

    // Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), purpleText));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

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

void volume_showcase() {
    hittable_list world;

    // Background elements - glowing spheres
    auto light_material = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(color(0.1, 0.1, 0.15))));
    
    // Create multiple colored smoke spheres with different densities
    
    // Large purple fog sphere
    auto boundary1 = make_shared<sphere>(point3(-2, 2, 0), 1.5, make_shared<dielectric>(1.5));
    world.add(boundary1);
    world.add(make_shared<volume>(boundary1, 0.5, color(0.8, 0.2, 0.9)));
    
    // Medium lime green smoke
    auto boundary2 = make_shared<sphere>(point3(2, 2, -1), 1.2, make_shared<dielectric>(1.5));
    world.add(boundary2);
    world.add(make_shared<volume>(boundary2, 0.8, color(0.4, 1.0, 0.2)));
    
    // Dense cyan fog ball
    auto boundary3 = make_shared<sphere>(point3(0, 3.5, 2), 1.0, make_shared<dielectric>(1.5));
    world.add(boundary3);
    world.add(make_shared<volume>(boundary3, 1.5, color(0.1, 0.9, 1.0)));
    
    // Small bright orange smoke
    auto boundary4 = make_shared<sphere>(point3(-1, 1, 3), 0.8, make_shared<dielectric>(1.5));
    world.add(boundary4);
    world.add(make_shared<volume>(boundary4, 1.2, color(1.0, 0.5, 0.1)));
    
    // Tiny pink smoke puff
    auto boundary5 = make_shared<sphere>(point3(1.5, 1.2, 2), 0.6, make_shared<dielectric>(1.5));
    world.add(boundary5);
    world.add(make_shared<volume>(boundary5, 2.0, color(1.0, 0.3, 0.7)));
    
    // Volumetric box - smoky purple cube
    shared_ptr<hittable> smoke_box = box(point3(-1, 0, -2), point3(0.5, 1.8, -0.5), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    smoke_box = make_shared<rotate_y>(smoke_box, 25);
    world.add(make_shared<volume>(smoke_box, 0.3, color(0.6, 0.1, 0.8)));
    
    // Light sources inside some volumes for glow effect
    world.add(make_shared<sphere>(point3(-2, 2, 0), 0.3, make_shared<diffuse_light>(color(10, 5, 12))));
    world.add(make_shared<sphere>(point3(2, 2, -1), 0.3, make_shared<diffuse_light>(color(6, 15, 3))));
    world.add(make_shared<sphere>(point3(0, 3.5, 2), 0.2, make_shared<diffuse_light>(color(2, 12, 15))));
    
    // Atmospheric fog - entire scene envelope
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 100, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0001, color(0.7, 0.8, 1.0)));
    
    // Some solid objects for contrast
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

void purple() {
    hittable_list world;

    // Soft ground fog
    auto ground = make_shared<lambertian>(color(0.15, 0.12, 0.2));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));
    
    // Large blue cloud masses
    auto blue_cloud1 = make_shared<sphere>(point3(-4, 3, -2), 3.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(blue_cloud1, 0.2, color(0.3, 0.4, 0.8)));
    
    auto blue_cloud2 = make_shared<sphere>(point3(5, 4, 1), 4.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(blue_cloud2, 0.15, color(0.2, 0.5, 0.9)));
    
    auto blue_cloud3 = make_shared<sphere>(point3(-2, 6, 5), 3.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(blue_cloud3, 0.25, color(0.4, 0.5, 0.95)));
    
    // Purple cloud masses
    auto purple_cloud1 = make_shared<sphere>(point3(3, 2, -3), 2.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_cloud1, 0.3, color(0.6, 0.3, 0.8)));
    
    auto purple_cloud2 = make_shared<sphere>(point3(-5, 5, 2), 3.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_cloud2, 0.18, color(0.7, 0.4, 0.9)));
    
    auto purple_cloud3 = make_shared<sphere>(point3(1, 7, -1), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(purple_cloud3, 0.22, color(0.5, 0.25, 0.7)));
    
    // Pink/magenta wisps
    auto pink_cloud1 = make_shared<sphere>(point3(2, 4, 3), 2.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(pink_cloud1, 0.28, color(0.9, 0.4, 0.7)));
    
    auto pink_cloud2 = make_shared<sphere>(point3(-3, 3, -1), 2.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(pink_cloud2, 0.35, color(0.95, 0.5, 0.8)));
    
    // Cyan/teal clouds
    auto cyan_cloud1 = make_shared<sphere>(point3(4, 5, 4), 2.6, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_cloud1, 0.2, color(0.3, 0.8, 0.9)));
    
    auto cyan_cloud2 = make_shared<sphere>(point3(-1, 4, -4), 2.3, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_cloud2, 0.25, color(0.2, 0.9, 0.95)));
    
    // Lavender mist layers
    auto lavender1 = make_shared<sphere>(point3(0, 2, 0), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(lavender1, 0.4, color(0.7, 0.6, 0.95)));
    
    auto lavender2 = make_shared<sphere>(point3(-2, 8, 3), 2.0, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(lavender2, 0.3, color(0.75, 0.65, 0.9)));
    
    // Scattered lantern/star lights - warm colors
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
    
    // More distant dimmer lights (stars)
    world.add(make_shared<sphere>(point3(-6, 12, -5), 0.15, make_shared<diffuse_light>(color(5, 5, 7))));
    world.add(make_shared<sphere>(point3(7, 11, -3), 0.18, make_shared<diffuse_light>(color(7, 4, 5))));
    world.add(make_shared<sphere>(point3(-2, 13, 4), 0.16, make_shared<diffuse_light>(color(4, 6, 8))));
    world.add(make_shared<sphere>(point3(6, 14, 1), 0.14, make_shared<diffuse_light>(color(6, 6, 4))));
    
    // Volumetric cloud layers for depth
    shared_ptr<hittable> cloud_box1 = box(point3(-7, 1, -5), point3(-2, 4, 3), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    cloud_box1 = make_shared<rotate_y>(cloud_box1, 25);
    world.add(make_shared<volume>(cloud_box1, 0.08, color(0.5, 0.4, 0.85)));
    
    shared_ptr<hittable> cloud_box2 = box(point3(2, 3, -4), point3(7, 7, 2), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    cloud_box2 = make_shared<rotate_y>(cloud_box2, -30);
    world.add(make_shared<volume>(cloud_box2, 0.1, color(0.4, 0.6, 0.9)));
    
    // Atmospheric haze - light purple/blue tint
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 200, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0001, color(0.5, 0.4, 0.7)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 400;
    cam.max_depth = 100;
    cam.background = color(0.15, 0.1, 0.25);  // Deep purple-blue

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

void dark() {
    hittable_list world;

    // Dark stormy ground
    auto dark_ground = make_shared<lambertian>(color(0.05, 0.05, 0.08));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, dark_ground));
    
    // Large turbulent cloud masses - dark grays and blacks
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
    
    // Smaller wispy clouds
    auto wisp1 = make_shared<sphere>(point3(1, 7, 1), 1.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(wisp1, 0.15, color(0.2, 0.18, 0.22)));
    
    auto wisp2 = make_shared<sphere>(point3(-2, 8, -1), 1.3, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(wisp2, 0.2, color(0.16, 0.14, 0.18)));
    
    // Dense storm core - very dark
    auto storm_core = make_shared<sphere>(point3(0, 4, 0), 1.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(storm_core, 1.5, color(0.02, 0.02, 0.03)));
    
    // Lightning-struck areas - subtle purple/blue glow
    auto charged1 = make_shared<sphere>(point3(-1, 5, -1), 0.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(charged1, 0.6, color(0.15, 0.12, 0.25)));
    
    auto charged2 = make_shared<sphere>(point3(2, 4, 1), 0.6, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(charged2, 0.8, color(0.12, 0.15, 0.3)));
    
    // Distant lightning flashes (dim lights)
    world.add(make_shared<sphere>(point3(-4, 6, -5), 0.5, make_shared<diffuse_light>(color(0.8, 0.9, 1.2))));
    world.add(make_shared<sphere>(point3(5, 7, -3), 0.3, make_shared<diffuse_light>(color(0.6, 0.7, 1.0))));
    world.add(make_shared<sphere>(point3(1, 9, 2), 0.4, make_shared<diffuse_light>(color(0.5, 0.6, 0.9))));
    
    // Volumetric cloud boxes - layered storm clouds
    shared_ptr<hittable> cloud_layer1 = box(point3(-8, 2, -6), point3(-3, 5, 2), make_shared<lambertian>(color(0.1, 0.1, 0.1)));
    cloud_layer1 = make_shared<rotate_y>(cloud_layer1, 15);
    world.add(make_shared<volume>(cloud_layer1, 0.1, color(0.14, 0.13, 0.16)));
    
    shared_ptr<hittable> cloud_layer2 = box(point3(3, 3, -4), point3(7, 6, 3), make_shared<lambertian>(color(0.1, 0.1, 0.1)));
    cloud_layer2 = make_shared<rotate_y>(cloud_layer2, -20);
    world.add(make_shared<volume>(cloud_layer2, 0.12, color(0.11, 0.1, 0.13)));
    
    // Very thin atmospheric haze - dark blue/purple tint
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 150, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.00008, color(0.1, 0.08, 0.15)));
    
    // A few barely visible stars (very dim lights far away)
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

void colorful() {
    hittable_list world;

    // Forest floor with texture
    auto forest_floor = make_shared<lambertian>(color(0.1, 0.15, 0.08));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, forest_floor));
    
    // Floating glowing orbs 
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
    
    // Colored mystical fog clouds
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
    
    // Glowing geometric crystals
    auto crystal_purple = make_shared<lambertian>(color(0.8, 0.4, 1.0));
    auto crystal_cyan = make_shared<lambertian>(color(0.3, 0.9, 1.0));
    auto crystal_lime = make_shared<lambertian>(color(0.7, 1.0, 0.3));
    
    // Crystal 
    shared_ptr<hittable> crystal1 = box(point3(-1.5, 0, -1), point3(-0.8, 1.8, -0.3), crystal_purple);
    crystal1 = make_shared<rotate_y>(crystal1, 25);
    world.add(crystal1);
    
    shared_ptr<hittable> crystal2 = box(point3(1, 0, 0.5), point3(1.6, 2.2, 1.1), crystal_cyan);
    crystal2 = make_shared<rotate_y>(crystal2, -35);
    world.add(crystal2);
    
    shared_ptr<hittable> crystal3 = box(point3(-0.3, 0, 2), point3(0.3, 1.5, 2.5), crystal_lime);
    crystal3 = make_shared<rotate_y>(crystal3, 15);
    world.add(crystal3);
    
    // Reflective spheres 
    auto glass = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(-2.5, 0.8, 1.5), 0.8, glass));
    world.add(make_shared<sphere>(point3(2.8, 0.6, -0.5), 0.6, glass));
    
    // Metal spheres with color
    world.add(make_shared<sphere>(point3(1.5, 0.5, -3), 0.5, make_shared<metal>(color(0.9, 0.5, 1.0), 0.1)));
    world.add(make_shared<sphere>(point3(-3, 0.7, -2.5), 0.7, make_shared<metal>(color(0.5, 1.0, 0.9), 0.05)));
    
    // Layered atmospheric fog
    shared_ptr<hittable> fog_layer1 = box(point3(-5, 0.2, -4), point3(-1, 2, 2), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_layer1 = make_shared<rotate_y>(fog_layer1, 20);
    world.add(make_shared<volume>(fog_layer1, 0.05, color(0.5, 0.7, 1.0)));
    
    shared_ptr<hittable> fog_layer2 = box(point3(1, 0.5, -3), point3(5, 3, 3), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_layer2 = make_shared<rotate_y>(fog_layer2, -25);
    world.add(make_shared<volume>(fog_layer2, 0.06, color(0.8, 0.5, 1.0)));
    
    // Ground mist
    auto ground_fog = make_shared<sphere>(point3(0, 0.3, 0), 8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(ground_fog, 0.02, color(0.4, 0.6, 0.8)));
    
    // Thin magical atmosphere
    auto atmosphere = make_shared<sphere>(point3(0, 0, 0), 100, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(atmosphere, 0.0002, color(0.4, 0.5, 0.7)));
    
    // Distant dim lights (like fireflies)
    world.add(make_shared<sphere>(point3(-5, 2, -3), 0.15, make_shared<diffuse_light>(color(8, 10, 5))));
    world.add(make_shared<sphere>(point3(5, 3, -2), 0.12, make_shared<diffuse_light>(color(10, 5, 8))));
    world.add(make_shared<sphere>(point3(-4, 6, 2), 0.18, make_shared<diffuse_light>(color(5, 12, 10))));
    world.add(make_shared<sphere>(point3(4, 7, 1), 0.14, make_shared<diffuse_light>(color(12, 8, 5))));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 700;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.background = color(0.05, 0.08, 0.15);  // Deep twilight blue

    cam.vfov = 50;
    cam.lookfrom = point3(0, 2, 10);
    cam.lookat = point3(0, 2.5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.3;  // Slight depth of field for dreamy effect
    cam.focus_dist = 10.0;

    if (g_use_opengl && g_window) {
        std::cout << "Calling render_opengl" << std::endl;
        cam.render_opengl(world, g_window, g_tex);
    } else {
        std::cout << "Calling regular render" << std::endl;
        cam.render(world);
    }
}

void liquidy_triangles() {
    hittable_list world;

    // Dark liquid floor
    auto liquid_floor = make_shared<lambertian>(color(0.05, 0.1, 0.15));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, liquid_floor));
    
    // Glowing liquid-colored lights
    auto aqua_glow = make_shared<diffuse_light>(color(8, 18, 20));
    auto teal_glow = make_shared<diffuse_light>(color(5, 20, 18));
    auto cyan_glow = make_shared<diffuse_light>(color(10, 22, 25));
    auto deep_blue_glow = make_shared<diffuse_light>(color(8, 15, 25));
    auto turquoise_glow = make_shared<diffuse_light>(color(12, 25, 22));
    
    // Floating light orbs like water droplets
    world.add(make_shared<sphere>(point3(-2, 4, -1), 0.4, aqua_glow));
    world.add(make_shared<sphere>(point3(2.5, 3.5, 0), 0.5, teal_glow));
    world.add(make_shared<sphere>(point3(0, 5, 2), 0.35, cyan_glow));
    world.add(make_shared<sphere>(point3(-3, 3, 1), 0.45, deep_blue_glow));
    world.add(make_shared<sphere>(point3(3, 4.5, -2), 0.38, turquoise_glow));
    
    // Smaller droplet lights
    world.add(make_shared<sphere>(point3(-1, 6, 0), 0.2, aqua_glow));
    world.add(make_shared<sphere>(point3(1.5, 5.5, -1), 0.25, teal_glow));
    world.add(make_shared<sphere>(point3(-2.5, 4.5, 2), 0.22, cyan_glow));
    
    // Liquid-colored translucent materials
    auto liquid_cyan = make_shared<lambertian>(color(0.2, 0.7, 0.8));
    auto liquid_teal = make_shared<lambertian>(color(0.2, 0.8, 0.7));
    auto liquid_aqua = make_shared<lambertian>(color(0.3, 0.9, 0.9));
    auto liquid_blue = make_shared<lambertian>(color(0.2, 0.6, 0.9));
    
    // Flowing triangular shards - like crystallized water
    // Diagonal flowing pattern
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
    
    // Glass water droplets
    auto glass = make_shared<dielectric>(1.33);  // Water's refractive index
    world.add(make_shared<sphere>(point3(-2, 0.6, 1), 0.6, glass));
    world.add(make_shared<sphere>(point3(2, 0.5, -1.5), 0.5, glass));
    world.add(make_shared<sphere>(point3(0, 0.4, -2), 0.4, glass));
    
    // Metallic liquid mercury-like spheres
    world.add(make_shared<sphere>(point3(-3.5, 0.5, -1), 0.5, make_shared<metal>(color(0.7, 0.9, 1.0), 0.02)));
    world.add(make_shared<sphere>(point3(3.5, 0.6, 1), 0.6, make_shared<metal>(color(0.6, 0.95, 0.95), 0.05)));
    
    // Watery mist clouds
    auto cyan_mist = make_shared<sphere>(point3(-1.5, 2, 0), 2.5, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(cyan_mist, 0.4, color(0.2, 0.7, 0.9)));
    
    auto teal_mist = make_shared<sphere>(point3(2, 2.5, -1), 2.8, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(teal_mist, 0.35, color(0.3, 0.9, 0.8)));
    
    auto aqua_mist = make_shared<sphere>(point3(-1, 3.5, 2), 2.2, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(aqua_mist, 0.38, color(0.4, 0.95, 0.95)));
    
    // Flowing fog layers
    shared_ptr<hittable> fog_flow1 = box(point3(-4, 0.3, -3), point3(-0.5, 2.5, 1), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_flow1 = make_shared<rotate_y>(fog_flow1, 30);
    world.add(make_shared<volume>(fog_flow1, 0.08, color(0.3, 0.8, 0.9)));
    
    shared_ptr<hittable> fog_flow2 = box(point3(0.5, 0.5, -2), point3(4, 3, 2), make_shared<lambertian>(color(0.5, 0.5, 0.5)));
    fog_flow2 = make_shared<rotate_y>(fog_flow2, -35);
    world.add(make_shared<volume>(fog_flow2, 0.07, color(0.4, 0.9, 0.85)));
    
    // Water surface mist
    auto surface_mist = make_shared<sphere>(point3(0, 0.2, 0), 6, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(surface_mist, 0.03, color(0.4, 0.8, 0.9)));
    
    // Underwater atmosphere effect
    auto water_atmosphere = make_shared<sphere>(point3(0, 0, 0), 80, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(water_atmosphere, 0.0005, color(0.3, 0.7, 0.9)));
    
    // Distant glowing water particles
    world.add(make_shared<sphere>(point3(-5, 3, -2), 0.15, make_shared<diffuse_light>(color(6, 15, 18))));
    world.add(make_shared<sphere>(point3(5, 4, -1), 0.18, make_shared<diffuse_light>(color(8, 18, 20))));
    world.add(make_shared<sphere>(point3(-4, 5, 2), 0.12, make_shared<diffuse_light>(color(10, 20, 18))));
    world.add(make_shared<sphere>(point3(4, 6, 1), 0.16, make_shared<diffuse_light>(color(7, 22, 22))));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 700;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.background = color(0.02, 0.08, 0.12);  // Deep underwater blue

    cam.vfov = 45;
    cam.lookfrom = point3(0, 2.5, 9);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.4;  // Watery depth of field
    cam.focus_dist = 9.0;

    if (g_use_opengl && g_window) {
        cam.render_opengl(world, g_window, g_tex);
    } else {
        cam.render(world);
    }
}

void triangle_test() {
    hittable_list world;

    // Floor
    auto floor_mat = make_shared<lambertian>(color(0.2, 0.2, 0.2));


    auto purpleTexture = make_shared<image_texture>("purpleTexture.jpg");
    auto purpleText = make_shared<lambertian>(purpleTexture);


    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, floor_mat));
    
    // Three tetrahedrons with different materials
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

void meshBasic(){
    hittable_list world;

    // Materials
    auto mesh_material = make_shared<lambertian>(color(0.8, 0.3, 0.3));
    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));

    // Add a ground sphere for reference
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    // Load the mesh - try different scales
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

void mesh(){
    hittable_list world;

    // Materials
    auto mesh_material = make_shared<lambertian>(color(0.128, 0, 0.128));
    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));

    // Add a ground sphere for reference
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));

    // Load the mesh - try different scales
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




void summon_image(){
    //lessSpheresFast();
    switch(24) {
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
        case 14: final_scene(400, 250, 4); break;
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
    }
}




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
        // Window is wider - pillarboxing
        vp_height = height;
        vp_width = (int)(height * img_aspect);
        vp_x = (width - vp_width) / 2;
        vp_y = 0;
    } else {
        // Window is taller - letterboxing
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
    
    // Set the callback
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
