#ifndef CUBE_MAPS_H
#define CUBE_MAPS_H

#include "rtweekend.h" 
#include "color.h"      
#include "rtw_stb_image.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>

#define TINYEXR_USE_MINIZ 0
#define TINYEXR_USE_THREAD 0       
#define TINYEXR_USE_OPENMP 0

#include "tinyexr.h"

#ifndef TINYEXR_USE_STB_ZLIB
extern "C" {
    inline int compress(unsigned char*, unsigned long*, const unsigned char*, unsigned long) { return 0; }
    inline int uncompress(unsigned char*, unsigned long*, const unsigned char*, unsigned long) { return 0; }
}
#endif

class cube_maps {
    public:
        enum face_index {
            POSITIVE_X = 0,  // Right
            NEGATIVE_X = 1,  // Left 
            POSITIVE_Y = 2,  // Top 
            NEGATIVE_Y = 3,  // Bottom
            POSITIVE_Z = 4,  // Front
            NEGATIVE_Z = 5   // Back
        };

        cube_maps() : resolution(0), channels(0), loaded(false), hdr(false) {
            for(int i = 0; i < 6; i++) {
                faces[i] = nullptr;
                face_data_float[i] = nullptr;
            }
        }
        
        cube_maps(const std::string file_paths[6]) 
            : resolution(0), channels(0), loaded(false), hdr(false) 
        {
            for(int i = 0; i < 6; i++) {
                faces[i] = nullptr;
                face_data_float[i] = nullptr;
            }
            load_from_files(file_paths);
        }

        cube_maps(const std::string& equirect_path, int face_resolution)
            : resolution(0), channels(0), loaded(false), hdr(false)
        {
            for(int i = 0; i < 6; i++) {
                faces[i] = nullptr;
                face_data_float[i] = nullptr;
            }
            load_from_equirectangular(equirect_path, face_resolution);
        }
        
        ~cube_maps() {
            cleanup();
        }

        bool load_from_files(const std::string file_paths[6]) {
            cleanup();
            
            for (int i = 0; i < 6; i++) {
                if (!load_face(static_cast<face_index>(i), file_paths[i])) {
                    std::cerr << "Failed to load face " << i << ": " << file_paths[i] << std::endl;
                    cleanup();
                    return false;
                }
            }
            
            loaded = validate_faces();
            return loaded;
        }
        
        bool load_from_equirectangular(const std::string& equirect_path, int face_resolution) {
            cleanup();
            resolution = face_resolution;
            
            if (is_exr_file(equirect_path)) {
                float* rgba_data = nullptr;
                const char* err = nullptr;
                int width, height;
                
                int ret = LoadEXR(&rgba_data, &width, &height, equirect_path.c_str(), &err);
                
                if (ret != TINYEXR_SUCCESS) {
                    if (err) {
                        std::cerr << "EXR Load Error: " << err << std::endl;
                        FreeEXRErrorMessage(err);
                    }
                    return false;
                }
                
                hdr = true;
                channels = 4;
                
                for (int i = 0; i < 6; i++) {
                    face_data_float[i] = (float*)malloc(resolution * resolution * channels * sizeof(float));
                }
                
                for (int i = 0; i < 6; i++) {
                    populate_face_from_equirect_hdr(rgba_data, width, height, static_cast<face_index>(i));
                }
                
                free(rgba_data);
                
            } else {
                int width, height, ch;
                unsigned char* data = stbi_load(equirect_path.c_str(), &width, &height, &ch, 0);
                
                if (!data) {
                    std::cerr << "Failed to load equirectangular image: " << equirect_path << std::endl;
                    return false;
                }
                
                hdr = false;
                channels = ch;
                
                for (int i = 0; i < 6; i++) {
                    faces[i] = (unsigned char*)malloc(resolution * resolution * channels * sizeof(unsigned char));
                }
                
                for (int i = 0; i < 6; i++) {
                    populate_face_from_equirect(data, width, height, static_cast<face_index>(i));
                }
                
                stbi_image_free(data);
            }
            
            loaded = validate_faces();
            return loaded;
        }
        
        bool load_face(face_index face, const std::string& file_path) {
            if (is_exr_file(file_path)) {
                float* rgba_data = nullptr;
                const char* err = nullptr;
                int width, height;
                
                int ret = LoadEXR(&rgba_data, &width, &height, file_path.c_str(), &err);
                
                if (ret != TINYEXR_SUCCESS) {
                    if (err) {
                        std::cerr << "EXR Load Error: " << err << std::endl;
                        FreeEXRErrorMessage(err);
                    }
                    return false;
                }
                
                if (resolution == 0) {
                    resolution = width;
                    channels = 4;
                    hdr = true;
                }
                
                if (width != resolution || height != resolution) {
                    std::cerr << "Face dimension mismatch: " << file_path << std::endl;
                    free(rgba_data);
                    return false;
                }
                
                if (face_data_float[face] != nullptr) {
                    free(face_data_float[face]);
                }
                
                face_data_float[face] = rgba_data;
                
            } else {
                int width, height, ch;
                unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &ch, 0);
                
                if (!data) {
                    std::cerr << "Failed to load image: " << file_path << std::endl;
                    return false;
                }
                
                if (resolution == 0) {
                    resolution = width;
                    channels = ch;
                    hdr = false;
                }
                
                if (width != resolution || height != resolution) {
                    std::cerr << "Face dimension mismatch: " << file_path << std::endl;
                    stbi_image_free(data);
                    return false;
                }
                
                if (faces[face] != nullptr) {
                    stbi_image_free(faces[face]);
                }
                
                faces[face] = data;
            }
            
            return true;
        }
        
        color sample(const vec3& direction) const {
            if (!loaded) {
                return color(0, 0, 0);
            }
            
            face_index face;
            double u, v;
            calculate_face_and_uv(direction, face, u, v);
            
            return sample_face(face, u, v);
        }
        
        bool is_valid() const {
            return loaded;
        }
        
        int get_resolution() const {
            return resolution;
        }
        
        bool is_hdr() const {
            return hdr;
        }
        
        void save_debug_unwrap(const std::string& output_path) const {
            if (!loaded) {
                std::cerr << "Cannot save debug unwrap: cube map not loaded" << std::endl;
                return;
            }
            
            int debug_width = resolution * 4;
            int debug_height = resolution * 3;
            std::vector<unsigned char> debug_image(debug_width * debug_height * 3, 0);
            
            auto copy_face = [&](face_index face, int offset_x, int offset_y) {
                for (int y = 0; y < resolution; y++) {
                    for (int x = 0; x < resolution; x++) {
                        color c;
                        if (hdr) {
                            c = get_pixel_color_hdr(face_data_float[face], x, y);
                            c = color(std::clamp(c.x(), 0.0, 1.0), 
                                     std::clamp(c.y(), 0.0, 1.0), 
                                     std::clamp(c.z(), 0.0, 1.0));
                        } else {
                            c = get_pixel_color(faces[face], x, y);
                        }
                        
                        int debug_x = offset_x + x;
                        int debug_y = offset_y + y;
                        int index = (debug_y * debug_width + debug_x) * 3;
                        
                        debug_image[index + 0] = static_cast<unsigned char>(c.x() * 255);
                        debug_image[index + 1] = static_cast<unsigned char>(c.y() * 255);
                        debug_image[index + 2] = static_cast<unsigned char>(c.z() * 255);
                    }
                }
            };
            
            copy_face(POSITIVE_Y, resolution, 0);
            copy_face(NEGATIVE_X, 0, resolution);
            copy_face(POSITIVE_Z, resolution, resolution);
            copy_face(POSITIVE_X, resolution * 2, resolution);
            copy_face(NEGATIVE_Z, resolution * 3, resolution);
            copy_face(NEGATIVE_Y, resolution, resolution * 2);
            
            stbi_write_png(output_path.c_str(), debug_width, debug_height, 3, debug_image.data(), debug_width * 3);
            std::cout << "Debug unwrap saved to: " << output_path << std::endl;
        }

    private:
        unsigned char* faces[6];
        float* face_data_float[6];
        int resolution;
        int channels;
        bool loaded;
        bool hdr;
        
        color sample_face(face_index face, double u, double v) const {
            if (hdr) {
                return bilinear_sample_hdr(face_data_float[face], u, v);
            } else {
                return bilinear_sample(faces[face], u, v);
            }
        }
        
        color bilinear_sample(const unsigned char* image_data, double u, double v) const {
            u = std::clamp(u, 0.0, 1.0);
            v = std::clamp(v, 0.0, 1.0);
            
            double x = u * (resolution - 1);
            double y = v * (resolution - 1);
            
            int x0 = static_cast<int>(std::floor(x));
            int y0 = static_cast<int>(std::floor(y));
            int x1 = std::min(x0 + 1, resolution - 1);
            int y1 = std::min(y0 + 1, resolution - 1);
            
            double fx = x - x0;
            double fy = y - y0;
            
            color c00 = get_pixel_color(image_data, x0, y0);
            color c10 = get_pixel_color(image_data, x1, y0);
            color c01 = get_pixel_color(image_data, x0, y1);
            color c11 = get_pixel_color(image_data, x1, y1);
            
            color c0 = c00 * (1.0 - fx) + c10 * fx;
            color c1 = c01 * (1.0 - fx) + c11 * fx;
            return c0 * (1.0 - fy) + c1 * fy;
        }
        
        color bilinear_sample_hdr(const float* image_data, double u, double v) const {
            u = std::clamp(u, 0.0, 1.0);
            v = std::clamp(v, 0.0, 1.0);
            
            double x = u * (resolution - 1);
            double y = v * (resolution - 1);
            
            int x0 = static_cast<int>(std::floor(x));
            int y0 = static_cast<int>(std::floor(y));
            int x1 = std::min(x0 + 1, resolution - 1);
            int y1 = std::min(y0 + 1, resolution - 1);
            
            double fx = x - x0;
            double fy = y - y0;
            
            color c00 = get_pixel_color_hdr(image_data, x0, y0);
            color c10 = get_pixel_color_hdr(image_data, x1, y0);
            color c01 = get_pixel_color_hdr(image_data, x0, y1);
            color c11 = get_pixel_color_hdr(image_data, x1, y1);
            
            color c0 = c00 * (1.0 - fx) + c10 * fx;
            color c1 = c01 * (1.0 - fx) + c11 * fx;
            return c0 * (1.0 - fy) + c1 * fy;
        }
        
        void calculate_face_and_uv(const vec3& direction, face_index& face, double& u, double& v) const {
            vec3 dir = unit_vector(direction);
            
            double abs_x = std::abs(dir.x());
            double abs_y = std::abs(dir.y());
            double abs_z = std::abs(dir.z());
            
            if (abs_x >= abs_y && abs_x >= abs_z) {
                if (dir.x() > 0) {
                    face = POSITIVE_X;
                    u = -dir.z() / abs_x;
                    v = -dir.y() / abs_x;
                } else {
                    face = NEGATIVE_X;
                    u = dir.z() / abs_x;
                    v = -dir.y() / abs_x;
                }
            } else if (abs_y >= abs_x && abs_y >= abs_z) {
                if (dir.y() > 0) {
                    face = POSITIVE_Y;
                    u = dir.x() / abs_y;
                    v = dir.z() / abs_y;
                } else {
                    face = NEGATIVE_Y;
                    u = dir.x() / abs_y;
                    v = -dir.z() / abs_y;
                }
            } else {
                if (dir.z() > 0) {
                    face = POSITIVE_Z;
                    u = dir.x() / abs_z;
                    v = -dir.y() / abs_z;
                } else {
                    face = NEGATIVE_Z;
                    u = -dir.x() / abs_z;
                    v = -dir.y() / abs_z;
                }
            }
            
            u = (u + 1.0) * 0.5;
            v = (v + 1.0) * 0.5;
        }
        
        void populate_face_from_equirect(const unsigned char* equirect_data, 
                                         int equirect_width, 
                                         int equirect_height, 
                                         face_index face) {
            for (int y = 0; y < resolution; y++) {
                for (int x = 0; x < resolution; x++) {
                    double s = (x + 0.5) / resolution * 2.0 - 1.0;
                    double t = (y + 0.5) / resolution * 2.0 - 1.0;
                    
                    vec3 dir = face_coords_to_direction(face, s, t);
                    
                    double theta = std::atan2(dir.z(), dir.x());
                    double phi = std::asin(dir.y());
                    
                    double u = (theta / (2.0 * pi) + 0.5);
                    double v = (phi / pi + 0.5);
                    
                    u = std::clamp(u, 0.0, 1.0);
                    v = std::clamp(v, 0.0, 1.0);
                    
                    int eq_x = static_cast<int>(u * (equirect_width - 1));
                    int eq_y = static_cast<int>(v * (equirect_height - 1));
                    
                    int src_index = (eq_y * equirect_width + eq_x) * channels;
                    int dst_index = (y * resolution + x) * channels;
                    
                    for (int c = 0; c < channels; c++) {
                        faces[face][dst_index + c] = equirect_data[src_index + c];
                    }
                }
            }
        }
        
        void populate_face_from_equirect_hdr(const float* equirect_data, 
                                             int equirect_width, 
                                             int equirect_height, 
                                             face_index face) {
            for (int y = 0; y < resolution; y++) {
                for (int x = 0; x < resolution; x++) {
                    double s = (x + 0.5) / resolution * 2.0 - 1.0;
                    double t = (y + 0.5) / resolution * 2.0 - 1.0;
                    
                    vec3 dir = face_coords_to_direction(face, s, t);
                    
                    double theta = std::atan2(dir.z(), dir.x());
                    double phi = std::asin(dir.y());
                    
                    double u = (theta / (2.0 * pi) + 0.5);
                    double v = (phi / pi + 0.5);
                    
                    u = std::clamp(u, 0.0, 1.0);
                    v = std::clamp(v, 0.0, 1.0);
                    
                    int eq_x = static_cast<int>(u * (equirect_width - 1));
                    int eq_y = static_cast<int>(v * (equirect_height - 1));
                    
                    int src_index = (eq_y * equirect_width + eq_x) * channels;
                    int dst_index = (y * resolution + x) * channels;
                    
                    for (int c = 0; c < channels; c++) {
                        face_data_float[face][dst_index + c] = equirect_data[src_index + c];
                    }
                }
            }
        }
        
        vec3 face_coords_to_direction(face_index face, double s, double t) const {
            vec3 dir;
            
            switch (face) {
                case POSITIVE_X:
                    dir = vec3(1.0, -t, -s);
                    break;
                case NEGATIVE_X:
                    dir = vec3(-1.0, -t, s);
                    break;
                case POSITIVE_Y:
                    dir = vec3(s, 1.0, t);
                    break;
                case NEGATIVE_Y:
                    dir = vec3(s, -1.0, -t);
                    break;
                case POSITIVE_Z:
                    dir = vec3(s, -t, 1.0);
                    break;
                case NEGATIVE_Z:
                    dir = vec3(-s, -t, -1.0);
                    break;
            }
            
            return unit_vector(dir);
        }
        
        bool validate_faces() const {
            if (resolution == 0) {
                return false;
            }
            
            if (hdr) {
                for (int i = 0; i < 6; i++) {
                    if (face_data_float[i] == nullptr) {
                        return false;
                    }
                }
            } else {
                for (int i = 0; i < 6; i++) {
                    if (faces[i] == nullptr) {
                        return false;
                    }
                }
            }
            
            return true;
        }
        
        void cleanup() {
            if (hdr) {
                for (int i = 0; i < 6; i++) {
                    if (face_data_float[i] != nullptr) {
                        free(face_data_float[i]);
                        face_data_float[i] = nullptr;
                    }
                }
            } else {
                for (int i = 0; i < 6; i++) {
                    if (faces[i] != nullptr) {
                        stbi_image_free(faces[i]);
                        faces[i] = nullptr;
                    }
                }
            }
            
            resolution = 0;
            channels = 0;
            loaded = false;
            hdr = false;
        }
        
        bool is_exr_file(const std::string& filename) const {
            return get_file_extension(filename) == "exr";
        }
        
        std::string get_file_extension(const std::string& filename) const {
            size_t dot_pos = filename.find_last_of('.');
            if (dot_pos == std::string::npos) {
                return "";
            }
            
            std::string ext = filename.substr(dot_pos + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), 
                           [](unsigned char c){ return std::tolower(c); });
            return ext;
        }
        
        color get_pixel_color(const unsigned char* data, int x, int y) const {
            int index = (y * resolution + x) * channels;
            
            double r = data[index + 0] / 255.0;
            double g = data[index + 1] / 255.0;
            double b = data[index + 2] / 255.0;
            
            return color(r, g, b);
        }
        
        color get_pixel_color_hdr(const float* data, int x, int y) const {
            int index = (y * resolution + x) * channels;
            
            double r = data[index + 0];
            double g = data[index + 1];
            double b = data[index + 2];
            
            return color(r, g, b);
        }
        
        void set_pixel_color(unsigned char* data, int x, int y, const color& c) {
            int index = (y * resolution + x) * channels;
            
            data[index + 0] = static_cast<unsigned char>(std::clamp(c.x(), 0.0, 1.0) * 255.0);
            data[index + 1] = static_cast<unsigned char>(std::clamp(c.y(), 0.0, 1.0) * 255.0);
            data[index + 2] = static_cast<unsigned char>(std::clamp(c.z(), 0.0, 1.0) * 255.0);
            
            if (channels == 4) {
                data[index + 3] = 255;
            }
        }
        
        void set_pixel_color_hdr(float* data, int x, int y, const color& c) {
            int index = (y * resolution + x) * channels;
            
            data[index + 0] = c.x();
            data[index + 1] = c.y();
            data[index + 2] = c.z();
            
            if (channels == 4) {
                data[index + 3] = 1.0f;
            }
        }
};

#endif