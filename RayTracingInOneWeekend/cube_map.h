#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#define _USE_MATH_DEFINES
#include <cmath>
#include "rtweekend.h"
#include <vector>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Forward declare stb_image
extern "C" {
    unsigned char* stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp);
    void stbi_image_free(void *retval_from_stbi_load);
}

class CubeMap {
public:
    std::vector<unsigned char> env_data;
    int env_width = 0;
    int env_height = 0;
    bool is_loaded = false;
    
    bool load_cubemap(const std::string& filepath) {
        std::cerr << "Loading environment map: " << filepath << std::endl;
        
        int w, h, comp;
        unsigned char* img = stbi_load(filepath.c_str(), &w, &h, &comp, 3);
        
        if (!img) {
            std::cerr << "Failed to load PNG environment map" << std::endl;
            return false;
        }
        
        env_width = w;
        env_height = h;
        env_data.resize(w * h * 3);
        memcpy(env_data.data(), img, w * h * 3);
        stbi_image_free(img);
        
        std::cerr << "Loaded environment map: " << w << "x" << h << std::endl;
        is_loaded = true;
        return true;
    }
    
    color sample_direction(const vec3& direction) const {
        if (!is_loaded) return color(0.5, 0.5, 0.5);
        
        vec3 d = unit_vector(direction);
        
        double x = d.x();
        double y = d.y();
        double z = d.z();
        
        // Convert direction to spherical coordinates
        double theta = std::atan2(z, x);
        double phi = std::acos(std::clamp(y, -1.0, 1.0));
        
        double u = (theta + M_PI) / (2.0 * M_PI);
        double v = phi / M_PI;
        
        u = std::clamp(u, 0.0, 0.999);
        v = std::clamp(v, 0.0, 0.999);
        
        int px = static_cast<int>(u * env_width);
        int py = static_cast<int>(v * env_height);
        
        int idx = (py * env_width + px) * 3;
        if (idx + 2 >= env_data.size()) return color(0.5, 0.5, 0.5);
        
        float r = env_data[idx] / 255.0f;
        float g = env_data[idx + 1] / 255.0f;
        float b = env_data[idx + 2] / 255.0f;
        
        // Apply gamma correction
        r = std::pow(r, 2.2f);
        g = std::pow(g, 2.2f);
        b = std::pow(b, 2.2f);
        
        return color(r, g, b);
    }
};

#endif