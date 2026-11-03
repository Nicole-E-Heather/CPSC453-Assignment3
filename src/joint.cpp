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
#include "joint.h"


void rotateShoulderJoint(Mesh& mesh, float radians)
{
    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::vec3(0.0f, 1.0f, 0.0f)));
}

void rotateElbowJoint(Mesh& mesh, float radians) {

}

void rotateWristJoint(Mesh& mesh, float radians) {

}

void rotateFingerJoint(Mesh& mesh, float radians) {

}

JointGeometry makeJoint(float radius)
{

    unsigned int slices = 64;
    unsigned int stacks = 32;

    constexpr float pi = 3.14159265358979323846f;
    JointGeometry sphere;
    sphere.vertices.reserve(static_cast<std::size_t>(stacks + 1) * (slices + 1));
    sphere.indices.reserve(static_cast<std::size_t>(stacks) * slices * 6);

    for (unsigned int stack = 0; stack <= stacks; ++stack) {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks);
        const float theta = pi * v;
        const float sinTheta = radius * (std::sin(theta));
        const float cosTheta = radius * (std::cos(theta));

        for (unsigned int slice = 0; slice <= slices; ++slice) {
            const float u = static_cast<float>(slice) / static_cast<float>(slices);
            const float phi = 2.0f * pi * u;
            const glm::vec3 normal(sinTheta * std::cos(phi),
                                   cosTheta,
                                   sinTheta * std::sin(phi));

            Vertex vertex;
            vertex.position = normal;
            vertex.normal = normal;
           // vertex.texCoord = glm::vec2(2*u, 1.0f - v);
            sphere.vertices.push_back(vertex);
        }
    }

    const unsigned int rowLength = slices + 1;
    for (unsigned int stack = 0; stack < stacks; ++stack) {
        for (unsigned int slice = 0; slice < slices; ++slice) {
            const unsigned int topLeft = stack * rowLength + slice;
            const unsigned int bottomLeft = topLeft + rowLength;
            const unsigned int topRight = topLeft + 1;
            const unsigned int bottomRight = bottomLeft + 1;

            sphere.indices.insert(sphere.indices.end(),
                                  {topLeft, topRight, bottomLeft,
                                   topRight, bottomRight, bottomLeft});
        }
    }

    return sphere;
}