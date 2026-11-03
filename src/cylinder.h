#include <SFML/Graphics.hpp>
#include <glad/gl.h>
#include <vector>
#include <iostream>

struct CylinderGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

void PitchCylinder(Mesh& mesh, float radians);
void RollCylinder(Mesh& mesh, float radians);
void YawCylinder(Mesh& mesh, float radians);

CylinderGeometry makeCylinder(float height, float radius, bool base);