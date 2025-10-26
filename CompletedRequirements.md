## Requirments for C-:

    [X] A camera with configurable position, orientation, and field of view -> Book 1

    [X] Anti-aliasing -> Book 1

    [X] Ray/sphere intersections -> Book 1

    [X] Ray/triangle intersections -> Not in Ray Tracing Series?

    [X] The ability to load textures (file format(s) of your choice; may use third-party libraries) -> Book 2

    [X] Textured spheres and triangles -> Triangles not in Ray Tracing Series

    [ ] The ability to load and render triangle meshes (file format(s) of your choice; may use third-party libraries for loading) -> Not in ray Tracing Series

    [X] A spatial subdivision acceleration structure of your choice -> Book 2

    [X] Specular, diffuse, and dielectric materials (per first volume of Ray Tracing in One Weekend series) -> Book 1

    [X] Emissive materials (lights) -> Book 2

## Must choose at least 25 for a C-, 75 for guaranteed A (would be over-exaguration):

    [ ] High dynamic range images: 10 -> Book 3

    [X] Volume rendering (smoke, clouds, etc.): 10 -> Book 2

    [X] Quads: 10 -> Book 2

    [ ] Quadrics: 15

    [ ] Spectral rendering: 30

    [ ] BRDF materials (Bi-directional reflectance distribution functions): 30 -> Partiall in book 3

    [ ] Subsurface scattering (BSSRDFs): 30

    [X] Motion blur: 10 -> Book 2

    [X] Defocus blur/depth of field: 10 -> Book 1
    
    [X] Object instancing: 10 -> Book 2, with quads/squares

    [X] Perlin noise: 10 -> Book 1 and Book 2

    [ ] Cube maps: 15 -> book 3

    [ ] Importance sampling: 15 -> Book 3

    [ ] Parallelization: 10 -> ?

    [ ] Normal interpolation (smooth shading): 5 -> Book 2

    [ ] Hybrid rendering with a GPU (OpenGL/DirectX + ray tracing): 20

    [ ] GPU acceleration (GPU computing w/ e.g., CUDA): 20

    [ ] Adaptive sampling: 15

## Do this weekend
    [ ] Quads and Triangles (Quads in Ray Tracing The Next Week. If Struggle with Triangles look at some of the supplemental materials provided by Schaffer and the triangle code we did in class)

## Due for Check-In after Quads and Triangles
    [ ] Lights (Chapter 7: Ray Tracing The Next Week)
    [ ] Instances (Chapter 8: Ray Tracing The Next Week)
    [ ] Volumes (Chapter 9: Ray Tracing The Next Week)
    [ ] Look into rendering/loading extrenal 3rd party triangle meshes (Maybe check other book)

## Due for Check-In After that
    [ ] Load/Render Triangle meshes stuff
    [ ] Importance Sampling? Depends on how long the meshes stuff takes

## Due After that (Aim for to complete this by Thanksgiving break so all I gotta worry about after that is the worst part - the paper)
    [ ] Finish Importance Sampling if did not get to
    [ ] Aim for about 20 more points from the extra list
        - High Dynamic Rane Images?
        - Parallelization?
        - Look an see if any other features have been added to the list and sound interesting
    [ ] IF importance sampling was completed previously, Look into picking some extra thing found online
        - Ideas: Read about a Skybox maybe? That could be cool
        - Toon/Non-Photorealistic Shading: Use the surface normal and light direction to compute brightness, then “step” the result into discrete color ranges (like 0.0–0.3 → dark, 0.3–0.7 → midtone, >0.7 → highlight).
        - Chromatic Abberration: A lens effect where red, green, and blue light refract differently, producing color fringing at edges. -> For refractive materials, vary the index of refraction slightly per color channel when tracing rays.
        - Progessive Rendering: Show a noisy image that refines as more samples are added. -> Render in passes and average results; update the display between passes.




