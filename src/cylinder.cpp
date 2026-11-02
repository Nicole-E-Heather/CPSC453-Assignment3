#include <glad/gl.h> // Must be first!
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "mesh.h"
#include "cylinder.h"

// Code based on sphere/cube starter code

void rotateCylinder(Mesh& mesh, float radians){
    // do whatever to rotate
}

CylinderGeometry makeCylinder(float height, float radius) {

    // can scale by raidus and height after
    
    // Start by making a flat circle with z = 0
    constexpr float pi = 3.14159265358979323846f;
    CylinderGeometry cyl;
    unsigned int slices_circle = 32;
    unsigned int slices_wall = 32; // 64 for now
    unsigned int stacks_wall = 64; // 

    cyl.vertices.reserve(static_cast<std::size_t>((slices_circle + 1) * (slices_circle + 1))); // plus 1 to close circle
    cyl.indices.reserve(static_cast<std::size_t>((slices_circle * 3) * 2));

    // Generate top 2D circle vetices
    for (unsigned int slice = 0; slice <= slices_circle; slice++) {
        // 0 because its a 2D surface
        const float v = 0.0f;
        // calculated using the 2D coordinates from slices and total slices
        const float u = static_cast<float>(slice) / static_cast<float>(slices_circle);
        // find the angle of the current slice
        const float theta = 2.0f * pi * u;
        // Create the vecotr with 3 points, (x, y, z)
        const glm::vec3 position(radius * std::sin(theta),
                                 0,
                                 radius * std::cos(theta));
        Vertex vertex;
        vertex.position = position;
        // Because this is the top, set normal vector facing up.
        vertex.normal = glm::vec3(0, 1, 0);
        // Text coordinates using u and v
        vertex.texCoord = glm::vec2(2*u, 1.0f - v);
        cyl.vertices.push_back(vertex);
    }

    // Allocate indices
    for (unsigned int i = 0; i < (slices_circle); i++) {
        // Center/First coordinate
        cyl.indices.push_back(0);
        // Second vertex
        cyl.indices.push_back(i);
        // Third Vertex
        cyl.indices.push_back(i+1);
    }

    // Generate Side walls
    for (unsigned int stack = 0; stack <= stacks_wall; stack++) {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks_wall);

        for (unsigned int slice = 0; slice <= slices_wall; slice++) {
            const float u = static_cast<float>(slice) / static_cast<float>(slices_wall);
            const float theta = 2.0f * pi * u;

            const glm::vec3 position(radius * std::sin(theta),
                                 -(height / stack),
                                 radius * std::cos(theta));
            Vertex vertex;
            vertex.position = position;
            // Because this is the top, set normal vector facing up.
            vertex.normal = glm::vec3(std::cos(theta), 0, std::sin(theta)); // Change
            // Text coordinates using u and v
            vertex.texCoord = glm::vec2(2*u, 1.0f - v);
            cyl.vertices.push_back(vertex);
        }
    } 

    // Add cylinder walls to indices
    const unsigned int rowLength = slices_wall + 1;
    for (unsigned int stack = 0; stack < stacks_wall; ++stack) {
        for (unsigned int slice = 0; slice < slices_wall; ++slice) {
        const unsigned int topLeft = stack * rowLength + slice;
        const unsigned int bottomLeft = topLeft + rowLength;
        const unsigned int topRight = topLeft + 1;
        const unsigned int bottomRight = bottomLeft + 1;

        cyl.indices.insert(cyl.indices.end(),
                                  {topLeft, topRight, bottomLeft,
                                   topRight, bottomRight, bottomLeft});
        }
    }

    // Dont have a bottom circle, but that shouldn't be too bad

    return cyl;
}

