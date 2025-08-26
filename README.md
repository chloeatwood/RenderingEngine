# RenderingEngine

## Building RayTracingInOneWeekend File
g++ -std=c++17 -O2 *.cpp -o inOneWeekend.exe
./inOneWeekend.exe > image.ppm
magick image.ppm image.png
