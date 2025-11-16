#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "hittable_list.h"
#include "triangle.h"
#include <iostream>

inline shared_ptr<hittable_list> load_obj_mesh(const std::string& filename, shared_ptr<material> mat, const point3& offset = point3(0,0,0), double scale = 1.0){
    auto mesh = make_shared<hittable_list>();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    //load the obj file
    bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());

    if(!warn.empty()){
        std::cout << "WARN: " << warn << std::endl;
    }

    if(!err.empty()){
        std::cerr << "ERR: " << err << std::endl;
    }

    if(!loaded){
        std::cerr << "Failed to load: " << filename << std::endl;
        return mesh;
    }

    std::cout << "Loading Mesh: " << filename << std::endl;
    std::cout << "  Vertices: " << attrib.vertices.size() / 3 << std::endl;

    //loop through shapes in the file
    for( size_t s = 0; s < shapes.size(); s++){
        size_t in_offset = 0;

        //loop over faces
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++){
            size_t fv = shapes[s].mesh.num_face_vertices[f];

            //only handling triangles
            if(fv != 3){
                std::cerr << "Warning: Non-Triangle face detected. Skipping... " << std::endl;

                in_offset += fv;
                continue;
            }

            //Get the vertices of the triangle
            point3 vertices[3];

            for(size_t v = 0; v < 3; v++){
                tinyobj::index_t idx = shapes[s].mesh.indices[in_offset + v];

                //get vertex position
                double vx = attrib.vertices[3 * idx.vertex_index + 0];
                double vy = attrib.vertices[3 * idx.vertex_index + 1];
                double vz = attrib.vertices[3 * idx.vertex_index + 2];

                //apply scale and offset
                vertices[v] = point3(vx * scale, vy * scale, vz * scale) + offset;

            }

            //create the trianlge
            mesh->add(make_shared<triangle>(mesh_tag{}, vertices[0], vertices[1], vertices[2], mat));

            in_offset += fv;
        }
    }

    std::cout << "Triangles loaded?: " << mesh->objects.size() << std::endl;

    return mesh;
}

#endif