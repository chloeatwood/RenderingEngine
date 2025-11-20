#ifndef CAMERA_H
#define CAMERA_H

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"
#include "hittable.h"
#include "material.h"
#include "cube_maps.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <atomic>
#include <algorithm>
#include <cmath>


#ifdef _OPENMP
    #include <omp.h>
#endif

class camera {
    public:
        double aspect_ratio = 1.0;  //Ratio of image width over height
        int image_width = 100;      //Rendered image width in pixel count
        int samples_per_pixel = 10; //Count of random samples for each pixel
        int max_depth = 10;         //Maximum number of ray bounces into scene

        double vfov = 90;           //Vertical view angle (field of view)
        point3 lookfrom = point3(0, 0, 0);  //Point camera is looking from
        point3 lookat = point3(0, 0, -1);    //Point camera is looking at
        vec3 vup = vec3(0, 1, 0);           //Camera-relative "up" direction

        double defocus_angle = 0;   //Variation angle of rays through each pixel
        double focus_dist = 10;     //Distance from camera lookfrom point to plane of perfect focus
        color background;           //background color
        double exposure = 1.0;      //HDR exposure control


        //Now outputs to a png file so I no longer have to pipe the output tp a ppm file
            //Then use magick to convert it into a png. Can click on the png and watch
            //it render as well
        void render(const hittable& world, const cube_maps* env_map = nullptr){
            initialize();
            
            // Test OpenMP
            #ifdef _OPENMP
                std::cout << "OpenMP enabled! Using " << omp_get_max_threads() << " threads.\n";

                int total_cores = omp_get_max_threads();
                int threads_to_use = std::max(1, total_cores - 3);
                omp_set_num_threads(threads_to_use);
                std::cout << "Using " << threads_to_use << " threads for rendering" << std::endl;
            #else
                std::cout << "OpenMP NOT enabled!\n";
            #endif
            
            const char* filename = "output.png";
            std::vector<unsigned char> image(image_width * image_height * 3);


            //Storing HDR values
            std::vector<color> hdr_image(image_width * image_height);
            
            // Tile size (16x16)
            const int tile_size = 16;
            int num_tiles_x = (image_width + tile_size - 1) / tile_size;
            int num_tiles_y = (image_height + tile_size - 1) / tile_size;
            int total_tiles = num_tiles_x * num_tiles_y;
            
            std::atomic<int> tiles_completed(0);
            
            // Tiles
            #pragma omp parallel for schedule(dynamic)
            for (int tile_idx = 0; tile_idx < total_tiles; tile_idx++) {
                int tile_y = tile_idx / num_tiles_x;
                int tile_x = tile_idx % num_tiles_x;
                
                int start_j = tile_y * tile_size;
                int end_j = std::min(start_j + tile_size, image_height);
                int start_i = tile_x * tile_size;
                int end_i = std::min(start_i + tile_size, image_width);
                
                // Render this tile
                for (int j = start_j; j < end_j; j++) {
                    for (int i = start_i; i < end_i; i++) {
                        color pixel_color(0, 0, 0);
                        for (int sample = 0; sample < samples_per_pixel; sample++) {
                            ray r = get_ray(i, j);
                            pixel_color += ray_color(r, max_depth, world, env_map);
                        }

                        hdr_image[j * image_width + i] = pixel_samples_scale * pixel_color;
                        
                        // auto r = pixel_samples_scale * pixel_color.x();
                        // auto g = pixel_samples_scale * pixel_color.y();
                        // auto b = pixel_samples_scale * pixel_color.z();
                        
                        // r = std::sqrt(r);
                        // g = std::sqrt(g);
                        // b = std::sqrt(b);
                        
                        // int index = (j * image_width + i) * 3;
                        // image[index + 0] = static_cast<unsigned char>(256 * std::clamp(r, 0.0, 0.999));
                        // image[index + 1] = static_cast<unsigned char>(256 * std::clamp(g, 0.0, 0.999));
                        // image[index + 2] = static_cast<unsigned char>(256 * std::clamp(b, 0.0, 0.999));
                    }
                }
                
                // Update progress
                tiles_completed++;
                if (tiles_completed % 10 == 0 || tiles_completed == total_tiles) {
                    #pragma omp critical
                    {
                        int scanlines_done = (tiles_completed * tile_size * tile_size) / image_width;
                        std::clog << "\rProgress: " << (100 * tiles_completed / total_tiles) 
                                << "% (" << tiles_completed << "/" << total_tiles << " tiles)" 
                                << std::flush;
                    }
                }
                
            }
            
            // Final write

            tone_map_to_image(hdr_image, image);
            stbi_write_png(filename, image_width, image_height, 3, image.data(), image_width * 3);
            std::clog << "\rDone. Saved to " << filename << "                    \n";
        }

        void render_opengl(const hittable& world, GLFWwindow* window, GLuint texture, const cube_maps* env_map = nullptr){
            initialize();
            
            #ifdef _OPENMP
                int total_cores = omp_get_max_threads();
                int threads_to_use = std::max(1, total_cores - 3);
                omp_set_num_threads(threads_to_use);
                std::cout << "OpenMP enabled! Using " << threads_to_use << " threads.\n";
            #endif
            
            extern int g_image_width;
            extern int g_image_height;
            g_image_width = image_width;
            g_image_height = image_height;
            
            glfwSetWindowSize(window, image_width, image_height);
            glViewport(0, 0, image_width, image_height);
            
            std::vector<unsigned char> image(image_width * image_height * 3, 0);

            std::vector<color> hdr_image(image_width * image_height);
            
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 
                        0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
            
            const int tile_size = 16;
            int num_tiles_x = (image_width + tile_size - 1) / tile_size;
            int num_tiles_y = (image_height + tile_size - 1) / tile_size;
            int total_tiles = num_tiles_x * num_tiles_y;
            
            //Create spiral tile order for spiral shaped rendering
            std::vector<int> tile_order = generate_spiral_order(num_tiles_x, num_tiles_y);
            
            std::atomic<int> tiles_completed(0);
            
            // Parallelize over spiral tiles
            #pragma omp parallel for schedule(dynamic)
            for (int idx = 0; idx < total_tiles; idx++) {
                int tile_idx = tile_order[idx];  
                
                int tile_y = tile_idx / num_tiles_x;
                int tile_x = tile_idx % num_tiles_x;
                
                int start_j = tile_y * tile_size;
                int end_j = std::min(start_j + tile_size, image_height);
                int start_i = tile_x * tile_size;
                int end_i = std::min(start_i + tile_size, image_width);
                
                // Render this tile
                for (int j = start_j; j < end_j; j++) {
                    for (int i = start_i; i < end_i; i++) {
                        color pixel_color(0, 0, 0);
                        for (int sample = 0; sample < samples_per_pixel; sample++) {
                            ray r = get_ray(i, j);
                            pixel_color += ray_color(r, max_depth, world, env_map);
                        }

                        hdr_image[j * image_width + i] = pixel_samples_scale *pixel_color;
                        
                        // auto r = pixel_samples_scale * pixel_color.x();
                        // auto g = pixel_samples_scale * pixel_color.y();
                        // auto b = pixel_samples_scale * pixel_color.z();
                        
                        // r = std::sqrt(r);
                        // g = std::sqrt(g);
                        // b = std::sqrt(b);
                        
                        // int index = (j * image_width + i) * 3;
                        // image[index + 0] = static_cast<unsigned char>(256 * std::clamp(r, 0.0, 0.999));
                        // image[index + 1] = static_cast<unsigned char>(256 * std::clamp(g, 0.0, 0.999));
                        // image[index + 2] = static_cast<unsigned char>(256 * std::clamp(b, 0.0, 0.999));
                    }
                }
                
                tiles_completed++;
                
                // Update display for spiral effect
                if (tiles_completed % 2 == 0 || tiles_completed == total_tiles) {
                    #pragma omp critical
                    {
                        std::clog << "\rProgress: " << (100 * tiles_completed / total_tiles) 
                                << "% (" << tiles_completed << "/" << total_tiles << " tiles)" 
                                << std::flush;

                        tone_map_to_image(hdr_image, image);
                        
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, 
                                        GL_RGB, GL_UNSIGNED_BYTE, image.data());
                        
                        glClear(GL_COLOR_BUFFER_BIT);
                        glEnable(GL_TEXTURE_2D);
                        glBegin(GL_QUADS);
                        glTexCoord2f(0, 1); glVertex2f(-1, -1);
                        glTexCoord2f(1, 1); glVertex2f( 1, -1);
                        glTexCoord2f(1, 0); glVertex2f( 1,  1);
                        glTexCoord2f(0, 0); glVertex2f(-1,  1);
                        glEnd();
                        
                        glfwSwapBuffers(window);
                        glfwPollEvents();
                    }
                }
            }
            
            tone_map_to_image(hdr_image, image);
            const char* filename = "output.png";
            stbi_write_png(filename, image_width, image_height, 3, image.data(), image_width * 3);
            
            std::clog << "\rDone. Saved to " << filename << "                    \n";
        }


    private:
        int image_height;   //Rendered image height
        point3 center;      //Camera center
        point3 pixel00_loc; //Location of pixel 0, 0
        vec3 pixel_delta_u; //Offest to pixel to the right
        vec3 pixel_delta_v; //Offset to pixel below
        double pixel_samples_scale; //Color scale factor for a sum of pixel samples
        vec3 u, v, w;               //Camera frame basis vectors
        vec3 defocus_disk_u;        //Defocus disk horizontal radius
        vec3 defocus_disk_v;        //Defocus disk vertical radius


        void initialize(){
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            center = lookfrom;

            //Determine viewport dimentions
            //auto focal_length = (lookfrom - lookat).length();
            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta/2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height*(double(image_width)/image_height);

            //Calculate the u, v, w unit basis vectors for the camera coordinate frame
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            //Calculate the vectors across the horizontal and down the vertical viewport edges
            auto viewport_u = viewport_width * u;   //Vector across viewport horizontal edge
            auto viewport_v = viewport_height * -v; //Vector down viewport vertical edge

            //Calculate the horizontal and vertical delta vectors from pixel to pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            //Calculate the location og the upper left pixel
            auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            //Calculate the camera defocus disk basis vectors
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(int i, int j) const {
            //Construct a camera ray originating from the origin and directed at randomly sampled
            //point around the pixel location i, j

            auto offset = sample_square();
            auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;
            auto ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        vec3 sample_square() const {
            //returns the cevtor to a random point in the [-.5, -.5] -[.5, .5] unit square
            return vec3(random_double() -0.5, random_double() - 0.5, 0);
        }

        point3 defocus_disk_sample() const {
            //returns a random point in the camera defocus disk
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world, const cube_maps* env_map = nullptr) const {
            //If we've exceeded the ray bounce limit, no more light is gathered
            if(depth <= 0){
                return color(0, 0, 0);
            }

            hit_record rec;

            if(!world.hit(r, interval(0.001, infinity), rec)){
                if(env_map != nullptr && env_map->is_valid()){
                    return env_map->sample(r.direction());
                }else{
                    return background;
                }
            }

            ray scattered;
            color attenuation;
            color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

            if(!rec.mat->scatter(r, rec, attenuation, scattered)){
                return color_from_emission;
            }

            color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, env_map);

            return color_from_emission + color_from_scatter;
        }



        //Spiral function to render pixels in a spiral shape
        std::vector<int> generate_spiral_order(int width, int height) const {
            std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
            std::vector<int> order;
            order.reserve(width * height);
            
            int x = width / 2;
            int y = height / 2;
            int dx = 0, dy = -1;  // Start going up
            
            int steps = 1;
            int step_count = 0;
            int direction_changes = 0;
            
            while (order.size() < width * height) {
                // Add current tile if valid
                if (x >= 0 && x < width && y >= 0 && y < height && !visited[y][x]) {
                    visited[y][x] = true;
                    order.push_back(y * width + x);
                }
                
                // Move to next position
                x += dx;
                y += dy;
                step_count++;
                
                // Change direction after completing steps
                if (step_count == steps) {
                    step_count = 0;
                    direction_changes++;
                    
                    // Rotate 90 degrees clockwise
                    int temp = dx;
                    dx = -dy;
                    dy = temp;
                    
                    // Increase steps every 2 direction changes
                    if (direction_changes % 2 == 0) {
                        steps++;
                    }
                }
            }
            
            return order;
        }

        color tone_map_aces(const color& hdr) const {
            const double a = 2.51;
            const double b = 0.03;
            const double c = 2.43;
            const double d = 0.59;
            const double e = 0.14;

            auto tonemap = [&](double x) {
                return std::clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
            };

            return color(
                tonemap(hdr.x()),
                tonemap(hdr.y()),
                tonemap(hdr.z())
            );  
        }

        void tone_map_to_image(const std::vector<color>& hdr_image, std::vector<unsigned char>& ldr_image) const {
            for(int j = 0; j < image_height; j++){
                for(int i = 0; i < image_width; i++){
                    int index = j * image_width + i;
                    color hdr = hdr_image[index] * exposure;

                    color mapped = tone_map_aces(hdr);

                    mapped = color(std::pow(mapped.x(), 1.0/2.2), std::pow(mapped.y(), 1.0/2.2), std::pow(mapped.z(), 1.0/2.2));

                    int img_idx = index * 3;
                    ldr_image[img_idx + 0] = static_cast<unsigned char>(256 * std::clamp(mapped.x(), 0.0, 0.999));
                    ldr_image[img_idx + 1] = static_cast<unsigned char>(256 * std::clamp(mapped.y(), 0.0, 0.999));
                    ldr_image[img_idx + 2] = static_cast<unsigned char>(256 * std::clamp(mapped.z(), 0.0, 0.999));
                }
            }
        }
};

#endif