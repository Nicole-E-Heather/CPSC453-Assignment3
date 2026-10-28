#include <SFML/Graphics.hpp>
#include <glad/gl.h>
#include <vector>
#include <iostream>

struct CylinderGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};