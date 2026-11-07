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

CylinderGeometry makeCylinder(float height, float radius, bool base) {
    
    // Start by making a flat circle with z = 0
    constexpr float pi = 3.14159265358979323846f;
    CylinderGeometry cyl;
    unsigned int slices_circle = 64;
    unsigned int slices_wall = 64; // 64 for now
    unsigned int stacks_wall = 32; // 

    cyl.vertices.reserve(static_cast<std::size_t>((slices_circle + 1) * (slices_circle + 1))); // plus 1 to close circle
    cyl.indices.reserve(static_cast<std::size_t>((slices_circle * 3) * 2));

    // Generate top 2D circle vetices: similar process to generating the slices of a shpere
    // the slice goes horizontally across the circles radius where the triangles fill in the
    // the shape on the 2D plane
    for (unsigned int slice = 0; slice <= slices_circle; slice++) {
        // calculated using the 2D coordinates from slices and total slices
        const float u = static_cast<float>(slice) / static_cast<float>(slices_circle);
        // find the angle of the current slice
        const float theta = 2.0f * pi * u;
        // Create the vector with 3 points, (x, y, z)
        const glm::vec3 position(radius * std::cos(theta),
                                 height,
                                 radius * std::sin(theta));
        Vertex vertex;
        vertex.position = position;
        // Because this is the top, set normal vector facing up.
        vertex.normal = glm::vec3(0, 1, 0);

        if (base == true) {
            // Texture exists on a 1.0 < u < 1.0 and 1.0 < v < 1.0 plane
            // Radius will always be 0.5 so we multiply sin and cos to get the x and y values
            // then add on 0.5 to ensure coordinates are shifted to match the texture 
            // i.e we start at (1/2, 1/2)
            vertex.texCoord = glm::vec2(0.5f + (0.5f * std::cos(theta)), 0.5f + (0.5f * std::sin(theta)));
        }

        cyl.vertices.push_back(vertex);
        // Handle indices: starting with 0 because the point of the triangle is always the center
    }

    const unsigned int center = 0;
    for (int slice = 0; slice < slices_circle; slice++) {
        const unsigned int topLeft = slice + 1; // one corner of the triangle
        const unsigned int topRight = slice + 2; // scond corner of the triangle

        cyl.indices.push_back(center);
        cyl.indices.push_back(topLeft);
        cyl.indices.push_back(topRight);
    }

    // Generate Side walls: Similar again to the sphere again, but our walls are not angled.
    for (unsigned int stack = 0; stack <= stacks_wall; stack++) {

        const float v = static_cast<float>(stack) / static_cast<float>(stacks_wall);

        for (unsigned int slice = 0; slice <= slices_wall; slice++) {
            //const float u = static_cast<float>(slice) / static_cast<float>(slices_wall);
            const float u = static_cast<float>(slice) / static_cast<float>(slices_wall);
            //const float angle_text = 2.0f * pi * slice;
            const float theta = 2.0f * pi * u;

            const glm::vec3 position(radius * std::cos(theta),
                                 height * v, // Height of the stack essetnially
                                 radius * std::sin(theta));
            Vertex vertex;
            vertex.position = position;
            // Normal vector points out
            vertex.normal = glm::vec3(std::cos(theta), 0, std::sin(theta));
            // Text coordinates using u and v
            if (base == false && radius != 0.045f) {
                vertex.texCoord = glm::vec2(u, v);
            }
            cyl.vertices.push_back(vertex);
        }
    } 

    // Add cylinder walls to indices, exact same as sphere from starter code
    // Just ensure that slices do not start at 0 since that is reserved for
    // top circle.
    const unsigned int rowLength = slices_wall + 1;
    for (unsigned int stack = 0; stack < stacks_wall; ++stack) {
        for (unsigned int slice = slices_circle + 1; slice < slices_wall + slices_circle + 1; ++slice) {

            const unsigned int topLeft = stack * rowLength + slice;
            const unsigned int bottomLeft = topLeft + rowLength;
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomRight = bottomLeft + 1;

        cyl.indices.insert(cyl.indices.end(),
                                  {topLeft, topRight, bottomLeft,
                                   topRight, bottomRight, bottomLeft});
        }
    }

    // Don't need bottom circle for our purposes

    return cyl;
}

