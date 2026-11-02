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


struct JointGeometry {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

JointGeometry makeJoint(float radius);

void rotateShoulderJoint(Mesh& mesh, float radians);
void rotateWristJoint(Mesh& mesh, float radians);
void rotateFingerJoint(Mesh& mesh, float radians);