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
#include  "rectangle.h"

// TODO: Surface normal for (Nx, Ny, Nz)?????

// Taken from cube.cpp
Vertex makeVertex(const glm::vec3& position, const glm::vec2& texCoord)
{
    Vertex vertex;
    vertex.position = position;
    vertex.texCoord = texCoord;
    return vertex;
}

void rotateRect(Mesh& mesh, float radians) {

    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))));
                                    
}

RectangleGeometry makeRectangle() {

    // Code based on implementation of cube.cpp

    constexpr glm::vec2 bottomLeft(0.0f, 0.0f);
    constexpr glm::vec2 bottomRight(1.0f, 0.0f);
    constexpr glm::vec2 topRight(1.0f, 1.0f);
    constexpr glm::vec2 topLeft(0.0f, 1.0f);

    RectangleGeometry rect;
    rect.vertices = {
        // Front     x,     y,     z
        makeVertex({-1.0f, -0.5f,  1.0f}, bottomLeft),
        makeVertex({ 1.0f, -0.5f,  1.0f}, bottomRight),
        makeVertex({ 1.0f,  0.5f,  1.0f}, topRight),
        makeVertex({-1.0f,  0.5f,  1.0f}, topLeft),
        // Back
        makeVertex({ 1.0f, -0.5f, -1.0f}, bottomLeft),
        makeVertex({-1.0f, -0.5f, -1.0f}, bottomRight),
        makeVertex({-1.0f,  0.5f, -1.0f}, topRight),
        makeVertex({ 1.0f,  0.5f, -1.0f}, topLeft),
        // Right
        makeVertex({ 1.0f, -0.5f,  1.0f}, bottomLeft),
        makeVertex({ 1.0f, -0.5f, -1.0f}, bottomRight),
        makeVertex({ 1.0f,  0.5f, -1.0f}, topRight),
        makeVertex({ 1.0f,  0.5f,  1.0f}, topLeft),
        // Left
        makeVertex({-1.0f, -0.5f, -1.0f}, bottomLeft),
        makeVertex({-1.0f, -0.5f,  1.0f}, bottomRight),
        makeVertex({-1.0f,  0.5f,  1.0f}, topRight),
        makeVertex({-1.0f,  0.5f, -1.0f}, topLeft),
        // Top
        makeVertex({-1.0f,  0.5f,  1.0f}, bottomLeft),
        makeVertex({ 1.0f,  0.5f,  1.0f}, bottomRight),
        makeVertex({ 1.0f,  0.5f, -1.0f}, topRight),
        makeVertex({-1.0f,  0.5f, -1.0f}, topLeft),
        // Bottom
        makeVertex({-1.0f, -0.5f, -1.0f}, bottomLeft),
        makeVertex({ 1.0f, -0.5f, -1.0f}, bottomRight),
        makeVertex({ 1.0f, -0.5f,  1.0f}, topRight),
        makeVertex({-1.0f, -0.5f,  1.0f}, topLeft)
    };

    rect.indices.reserve(36);

    for (unsigned int face = 0; face < 6; ++face) {
        const unsigned int first = face * 4;
        rect.indices.insert(rect.indices.end(),
                            {first, first + 1, first + 2,
                             first, first + 2, first + 3});
    }

    return rect;
    
}