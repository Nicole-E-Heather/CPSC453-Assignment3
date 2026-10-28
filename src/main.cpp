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

#include "rectangle.h"
#include "cylinder.h"

// At some point sphere will need to be modified to allow header files

int main(int argc, char* argv[]) {

    // Code based off of repository examples

    try {

        sf::ContextSettings settings;
        settings.majorVersion = 4;
        settings.minorVersion = 0;
        settings.attributeFlags = sf::ContextSettings::Core;
        settings.depthBits = 24;

        sf::Window window(sf::VideoMode({800, 800}),
                          "Robot Arm",
                          sf::Style::Default,
                          sf::State::Windowed,
                          settings);

        const sf::ContextSettings actualSettings = window.getSettings();
        if (actualSettings.majorVersion < 4) {
            throw std::runtime_error("OpenGL 4.0 Core is required, but the created context is " +
                                     std::to_string(actualSettings.majorVersion) + "." +
                                     std::to_string(actualSettings.minorVersion));
        }

        // Ignore Error
        if (!gladLoadGL(sf::Context::getFunction)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return 1;
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Want to start with modelling the hand since its a rectangle, similar to cube
        RectangleGeometry rect = makeRectangle();

        const std::filesystem::path shaderDirectory =
        std::filesystem::absolute(argv[0]).parent_path() / "shaders";
        Material material;

        //std::cout << shaderDirectory << std::endl;

        Mesh mesh(rect.vertices,
                  rect.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  argv[1],
                  material);
        // Need to provide png for now

        constexpr float fieldOfView = glm::radians(45.0f);
        constexpr float cubeRadius = 1.73205080757f;
        const glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

        while (window.isOpen()) {

            while (const std::optional event = window.pollEvent()) {
                
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                } 

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
    
                    // Base Roll
                    if (keyPressed->scancode == sf::Keyboard::Scan::A) {
                        // Rotate Left
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::D) {
                        // Rotate Right
                    }

                    // Shoulder Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::W) {
                        // Up
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::S) {
                        // Down
                    }

                    // Elbow Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::R) {
                        // Up Relative to Forearm
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::F) {
                        // Down Relative to Forearm
                    }

                    // Wrist Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::T) {
                        // Rotate hand up
                        rotateRect(mesh, glm::radians(-5.0f));
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::G) {
                        // Rotate Hand down
                        rotateRect(mesh, glm::radians(5.0f));
                    }

                    // Gripper Claw
                    if (keyPressed->scancode == sf::Keyboard::Scan::O) {
                        // Open Claw
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::C) {
                        // Close Claw
                    }

                    // Reset Pose
                    if (keyPressed->scancode == sf::Keyboard::Scan::Space) {
                        // All joints reset to 0 degrees

                    }
                }
            }

            // Drawing mesh

            // Hand is facing forward right now, will need to change camera later
            const sf::Vector2u size = window.getSize();

            glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));

            const float aspectRatio = size.x == 0 || size.y == 0
                ? 1.0f
                : static_cast<float>(size.x) / static_cast<float>(size.y);

            const float limitingAspect = std::min(aspectRatio, 1.0f);

            const float limitingHalfFov = std::atan(
                std::tan(fieldOfView * 0.5f) * limitingAspect);

            const float cameraDistance = cubeRadius / std::sin(limitingHalfFov) * 1.1f;

            const glm::mat4 view = glm::lookAt(cameraTarget + glm::vec3(0.0f,
                                                                        0.0f,
                                                                        cameraDistance),
                                               cameraTarget,
                                               worldUp);

            const glm::mat4 projection = glm::perspective(fieldOfView,
                                                           aspectRatio,
                                                           0.1f,
                                                           cameraDistance + cubeRadius * 2.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mesh.draw(view, projection);

            window.display();
        }

    } catch (const std::exception& error) {
        std::cerr << "Application Failure" << std::endl;
        return 1;
    }

}