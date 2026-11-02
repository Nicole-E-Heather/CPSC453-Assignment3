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
Vertex makeVertex(const glm::vec3& position, const glm::vec2& texCoord, int face)
{
    Vertex vertex;
    vertex.position = position;
    //vertex.normal = position;
    vertex.texCoord = texCoord;

    if (face == 0) { // Front
        vertex.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    else if (face == 1) { // Back
        vertex.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    } 
    else if (face == 2) { // Right
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    else if (face == 3) { // Left
        vertex.normal = glm::vec3(0.0f, -1.0f, 0.0f);
    }
    else if (face == 4) { // Top
        vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else { // Bottom
        vertex.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    return vertex;
}

void PitchRect(Mesh& mesh, float radians) {

    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
                                    // Rotation of 0, 1, 0 for up and down only
}

void RollRect(Mesh& mesh, float radians) {
    
    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))));
}

void YawRect(Mesh& mesh, float radians) {
    mesh.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    radians,
                                    glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f))));

}

void translateRect(Mesh& mesh) {
    
    mesh.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)));

}

RectangleGeometry makeRectangle(float l, float w, float h) {

    // Code based on implementation of cube.cpp

    // TODO: the normals for each vertex should be the same for every face

   
    // Do these values need to change to the range of l, w and h?
    constexpr glm::vec2 bottomLeft(0.0f, 0.0f);
    constexpr glm::vec2 bottomRight(1.0f, 0.0f);
    constexpr glm::vec2 topRight(1.0f, 1.0f);
    constexpr glm::vec2 topLeft(0.0f, 1.0f);

    RectangleGeometry rect;
    rect.vertices = {
        // Front     x,     y,     z
        makeVertex({-l, -w,  h}, bottomLeft, 0),
        makeVertex({ l, -w,  h}, bottomRight, 0),
        makeVertex({ l,  w,  h}, topRight, 0),
        makeVertex({-l,  w,  h}, topLeft, 0),
        // Back
        makeVertex({ l, -w, -h}, bottomLeft, 1),
        makeVertex({-l, -w, -h}, bottomRight, 1),
        makeVertex({-l,  w, -h}, topRight, 1),
        makeVertex({ l,  w, -h}, topLeft, 1),
        // Right
        makeVertex({ l, -w,  h}, bottomLeft, 2),
        makeVertex({ l, -w, -h}, bottomRight, 2),
        makeVertex({ l,  w, -h}, topRight, 2),
        makeVertex({ l,  w,  h}, topLeft, 2),
        // Left
        makeVertex({-l, -w, -h}, bottomLeft, 3),
        makeVertex({-l, -w,  h}, bottomRight, 3),
        makeVertex({-l,  w,  h}, topRight, 3),
        makeVertex({-l,  w, -h}, topLeft, 3),
        // Top
        makeVertex({-l,  w,  h}, bottomLeft, 4),
        makeVertex({ l,  w,  h}, bottomRight, 4),
        makeVertex({ l,  w, -h}, topRight, 4),
        makeVertex({-l,  w, -h}, topLeft, 4),
        // Bottom
        makeVertex({-l, -w, -h}, bottomLeft, 5),
        makeVertex({ l, -w, -h}, bottomRight, 5),
        makeVertex({ l, -w,  h}, topRight, 5),
        makeVertex({-l, -w,  h}, topLeft, 5)
    };

    rect.indices.reserve(36);

    for (unsigned int face = 0; face < 6; ++face) {
        const unsigned int first = face * 4;
        rect.indices.insert(rect.indices.end(),
                            {first, first + 1, first + 2,
                             first, first + 2, first + 3});
    }

    //std::cout << "Vertices: " << rect.vertices.size() << std::endl;
    //std::cout << "Indices: " << rect.indices.size() << std::endl;

    return rect;
    
}