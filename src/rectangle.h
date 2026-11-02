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

RectangleGeometry makeRectangle(float l, float w, float h);
void translateRect(Mesh& mesh);

void PitchRect(Mesh& mesh, float radians);
void RollRect(Mesh& mesh, float radians);
void YawRect(Mesh& mesh, float radians);