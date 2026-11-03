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
#include "joint.h"


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

        const std::filesystem::path shaderDirectory =
        std::filesystem::absolute(argv[0]).parent_path() / "shaders";
        Material textured; // Can maybe use the same material?
        Material untextured;

        textured.shininess = 64.0f;

        untextured.ambient = glm::vec3(1.0f);
        untextured.shininess = 64.0f;

        //Untextured Components: Assign appropriate material properties 
        // (ambient, diffuse, specular coefficients, and shininess exponent) 
        //to non-textured elements (Joint spheres, Palm, Stems, Fingers) as desired.
    
        // Start by generating cylinder (base), centered at origin (0,0,0) from the 
        // no translation required 0.70 / 1.10

        glm::mat4 baseMatrix = glm::mat4(1.0f);

        // This matrix is the "world/base" matrix in which all starting positions should be applied to
        CylinderGeometry base = makeCylinder(1.10f, 0.70f, true);
        Mesh meshBase(base.vertices,
                  base.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  textured);
        float height = 5.0f;
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.f, height, 0.f));
        meshBase.applyTransform(baseMatrix);

      // meshBase.applyTransform(nes, glm::vec3(0.f, height, 0.f)));

        // set the parent/model matrix
        // need to change normals for spheres
        
        // Generate sphere (shoulderJoint), with origin positioned at the top center of the base
        // Should be translated to match (0,0,0.70), 0.24
        JointGeometry shoulderJoint = makeJoint(0.24f);
        Mesh meshShoulderJoint(shoulderJoint.vertices,
                  shoulderJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshShoulderJoint.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, height, 0.f)));

        // Generate cylinder (upperArm), with origin set to position on top of shoulder joint
        // Should be translated to match (0,0,0.70), 2.80 / 0.22
        CylinderGeometry upperArm = makeCylinder(2.80f, 0.22f, false);
        Mesh meshUpperArm(upperArm.vertices,
                  upperArm.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_arm_linkage.png",
                  textured);
        height += 2.80f;
        meshUpperArm.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, height, 0.f)));

        // Generate sphere (elbowJoint), with origin set to top of upper arm
        // Should be translated to match (0,0,(0.70+2.80)), 0.20
        JointGeometry elbowJoint = makeJoint(0.20f);
        Mesh meshElbowJoint(elbowJoint.vertices,
                  elbowJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshElbowJoint.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, height, 0.f)));

        // Generate cylinder (forearm), with origin top of elbowJoint/upperArm
        // Should be translated to match (0,0,(0.70+2.80)), 2.25 / 0.17
        CylinderGeometry forearm = makeCylinder(2.25f, 0.17f, false);
        Mesh meshForearm(forearm.vertices,
                  forearm.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_arm_linkage.png",
                  textured);
        height += 2.25f;
        meshForearm.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, height, 0.f)));


        // Generate sphere (wristJoint), with origin to match top of forearm
        // Should be translated to match (0,0,(0.70+2.80+2.25))
        JointGeometry wristJoint = makeJoint(0.17f);
        Mesh meshWristJoint(wristJoint.vertices,
                  wristJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
       // height += 0.10f;
        meshWristJoint.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, height, 0.f)));

        // Generate Rectangle (hand), with origin to match top of wristJoint/Forearm
        // Should be translated to match (0,0,(0.70+2.80+2.25)), 0.17
        // The hand is somewhat transparent?
        RectangleGeometry hand = makeRectangle(0.41f, 0.18f, 0.34f);
        Mesh meshHand(hand.vertices,
                  hand.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshHand.applyTransform(glm::rotate(glm::mat4(1.0f), 
                                glm::radians(45.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f)));
        float xDirection = -0.4f;
        meshHand.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(xDirection, height, 0.0f)));

        // Generate cylinders (stem1 and stem2), 0.58 / 0.045
        // Should have its own base translated to match end of hand (0,0,(0.70+2.80+2.25+0.82))
        CylinderGeometry stem1 = makeCylinder(0.58f, 0.045f, false);
        Mesh meshStem1(stem1.vertices,
                  stem1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        //height += 0.18f;
        xDirection += -0.30;
        meshStem1.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(xDirection, height, 0.f)));
        meshStem1.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    glm::radians(-90.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f)));

        CylinderGeometry stem2 = makeCylinder(0.58f, 0.045f, false);
        Mesh meshStem2(stem2.vertices,
                  stem2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        float yDirection = 0.35f;
        meshStem2.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, height, yDirection)));
        meshStem2.applyTransform(glm::rotate(glm::mat4(1.0f),
                                    glm::radians(-90.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f)));

        // Generate Spheres (fingerJoints1 and fingerJoints2), 0.58 / 0.045
        // Should have its base tranlated to match the end of stems (0,0,(0.70+2.80+2.25+0.82+0.58))
        JointGeometry fingerJoint1 = makeJoint(0.06f);
        Mesh meshFingerJoint1(fingerJoint1.vertices,
                  wristJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        height += 0.58f;
        meshFingerJoint1.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.05f, height, 0.f)));

        JointGeometry fingerJoint2 = makeJoint(0.06f);
        Mesh meshFingerJoint2(fingerJoint2.vertices,
                  wristJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshFingerJoint2.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, height, 0.05f)));

        // Generate Rectangles (finger1 and finger2), 0.06
        // Base should be translated to match end of stems (0,0,(0.70+2.80+2.25+0.82+0.58))
        RectangleGeometry finger1 = makeRectangle(0.41f, 0.18f, 0.34f);
        Mesh meshFinger1(finger1.vertices,
                  finger1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        height += 0.06;
        meshFinger1.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, height, 0.f)));

        RectangleGeometry finger2 = makeRectangle(0.41f, 0.18f, 0.34f);
        Mesh meshFinger2(finger2.vertices,
                  finger2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshFinger2.applyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, height, 0.05f)));

        // Anytime a parent component moves, the children components that are effected should be
        // updated to ensure there positions are still correct         

        constexpr float fieldOfView = glm::radians(45.0f);
        constexpr float cubeRadius = 8.f;
        const glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
        const glm::vec3 worldUp(0.0f, 5.0f, 0.0f);

        while (window.isOpen()) {

            while (const std::optional event = window.pollEvent()) {
                
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                } 

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
    
                    // Base Roll
                    if (keyPressed->scancode == sf::Keyboard::Scan::A) {
                        // Rotate Left
                        PitchRect(meshBase, glm::radians(-2.0f));
                        PitchCylinder(meshUpperArm, glm::radians(-2.0f));
                        PitchCylinder(meshForearm, glm::radians(-2.0f));

                        // rotating the hand is weird right now
                        // different axes I think
                        //PitchRect(meshHand, glm::radians(-2.0f));
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::D) {
                        // Rotate Right
                        PitchRect(meshBase, glm::radians(2.0f));
                        PitchCylinder(meshUpperArm, glm::radians(2.0f));
                        PitchCylinder(meshForearm, glm::radians(2.0f));

                        // translate back to origin

                        // rotate aroudn desired axis

                        // translate back to basic position
                        

                       // PitchRect(meshHand, glm::radians(2.0f));
                    }

                    // Shoulder Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::W) {
                        // Up
                        RollCylinder(meshUpperArm, glm::radians(-1.0f));

                        // Rotation should occurr on the on the joint, not the arm
                        // Once the joint it being rotated the arm should move with it
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::S) {
                        // Down
                        RollCylinder(meshUpperArm, glm::radians(1.0f));
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
                       // YawRect(meshHand, glm::radians(-2.0f));
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::G) {
                        // Rotate Hand down
                       // YawRect(meshHand, glm::radians(2.0f));
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

                        //translateRect(meshHand);

                    }
                }
            }

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


            meshBase.draw(view, projection);
            meshShoulderJoint.draw(view, projection);
            meshUpperArm.draw(view, projection);
            meshElbowJoint.draw(view, projection);
            meshForearm.draw(view, projection);
            meshWristJoint.draw(view, projection);
            //meshHand.draw(view, projection);
            //meshStem1.draw(view, projection);
            //meshStem2.draw(view, projection);

            window.display();
        }

    } catch (const std::exception& error) {
        std::cerr << "Application Failure: " << error.what() << std::endl;
        return 1;
    }

}