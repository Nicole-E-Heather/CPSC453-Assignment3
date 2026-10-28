// Header file for Hand.cpp
#include <SFML/Graphics.hpp>
#include <glad/gl.h>
#include <vector>
#include <iostream>

// Code based on sphere/cube starter code
struct RectangleGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

RectangleGeometry makeRectangle();
void rotateRect(Mesh& mesh, float radians);
