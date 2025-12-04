# Ray Tracer

## Building RayTracing File
    Navigate to the RayTracerImplementation folder
    make — builds the executable
    make run — builds (if needed) and runs it
    make clean — deletes the executable

## List of Functionalities Implementented in this Ray Tracer
    - A camera with configurable position, orientation, and field of view
    - Anti-aliasing
    - Ray/sphere intersections
    - Ray/triangle intersections
    - The ability to load textures 
        - Loads PNG and JPG files
        - Uses external libraries:
            - std_image.h
            - std_image_write.h
    - Textured spheres and triangles
    - The ability to load and render triangle meshes
        - Loads .OBJ files
        - Uses external library:
            - tiny_obj_loader.h
    - A spatial subdivision acceleration structure
        - BVH
    - Specular, diffuse, and dielectric materials
    - Emissive materials (lights)
    - High dynamic range images
    - Volume rendering (smoke, clouds, etc.)
    - Quads
    - Motion blur
    - Defocus blur/depth of field
    - Object instancing
    - Perlin noise
    - Cube maps
        - Loads PNG files
        - Uses external libraries:
            - std_image.h
            - std_image_write.h
    - Parallelization
        - Uses OpenMP
    - Normal interpolation (smooth shading)
