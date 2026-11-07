# CMake SFML Project Template (with 3D OpenGL)

Use this template repository for SFML windowing and input with 3D rendering handled by OpenGL.

This template is loosely based on the [original SFML CMake template](https://github.com/SFML/cmake-sfml-project). SFML is used for window creation and interaction; rendering is handled by OpenGL. Audio, networking, and SFML graphics components are disabled.

OpenGL is loaded via [GLAD](https://wikis.khronos.org/opengl/OpenGL_Loading_Library), and GLM supplies model, view, and projection matrices. Executables use an OpenGL 4.0 Core context and the GLSL shaders are copied by CMake into a `shaders` directory beside each executable.

# For This Assignment

The files created/modfied for this assignment are main.cpp, joint.cpp, cylinder.cpp and rectangle.cpp. This includes their coresponding header files.
CMakeLists.txt was modified for proper compilation.

## In order to run use the following commands:
cmake -B build 

cmake --build build

./build/bin/Debug/main   -- or --   ./build/bin/main

## Robot Arm Interaction Controls

Base Roll: A - left, D - right

Shoulder Pitch: W - up, S - down

Elbow Pitch: R - up, F - down

Wrist Pitch: T - up, G - down

Claw: O - open, C - close

Reset Pose: Resets joint angles
