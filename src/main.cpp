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

// Each segment stores its matrices and geoetry to make 
// subsequent transformations during runtime easier
struct Segment {

    unsigned int partIndex; // indices to keep track just in case

    glm::mat4 segmentMatrix; // any shape
    glm::mat4 parentMatrix; // Base, joint arm sphere ect
    glm::mat4 originalMatrix; // the matrix constructed at first segment generation
    // original is never modified unlike segment, used when arm is reset
};

// Store in a vector for easy access
struct RobotArm {
    std::vector<Segment> Arm;
};

RobotArm arm; // global for easy access


void createSegment(unsigned int partIndex, glm::mat4 segment, glm::mat4 parent) {

    Segment newSegment = {
        partIndex,
        segment,
        parent,
        segment
    };

    arm.Arm.push_back(newSegment);
}

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
        Material textured;
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
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.f, 0.0f, 0.f));
        meshBase.applyTransform(baseMatrix);
        createSegment(0, baseMatrix, baseMatrix);
        
        // Generate sphere (shoulderJoint), with origin positioned at the top center of the base
        // Should be translated to match (0,0,0.70), 0.24
        JointGeometry shoulderJoint = makeJoint(0.24f);
        Mesh meshShoulderJoint(shoulderJoint.vertices,
                  shoulderJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshShoulderJoint.applyTransform(baseMatrix);
        createSegment(1, baseMatrix, baseMatrix);

        // Generate cylinder (upperArm), with origin set to position on top of shoulder joint
        // Should be translated to match (0,0,0.70), 2.80 / 0.22
        CylinderGeometry upperArm = makeCylinder(2.80f, 0.22f, false);
        Mesh meshUpperArm(upperArm.vertices,
                  upperArm.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_arm_linkage.png",
                  textured);
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 2.80f, 0.0f));
        meshUpperArm.applyTransform(baseMatrix);
        createSegment(2, baseMatrix, arm.Arm.at(2-1).segmentMatrix);

        // Generate sphere (elbowJoint), with origin set to top of upper arm
        // Should be translated to match (0,0,(0.70+2.80)), 0.20
        JointGeometry elbowJoint = makeJoint(0.20f);
        Mesh meshElbowJoint(elbowJoint.vertices,
                  elbowJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshElbowJoint.applyTransform(baseMatrix);
        createSegment(3, baseMatrix, arm.Arm.at(3-1).segmentMatrix);

        // Generate cylinder (forearm), with origin top of elbowJoint/upperArm
        // Should be translated to match (0,0,(0.70+2.80)), 2.25 / 0.17
        CylinderGeometry forearm = makeCylinder(2.25f, 0.17f, false);
        Mesh meshForearm(forearm.vertices,
                  forearm.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_arm_linkage.png",
                  textured);
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 2.25f, 0.0f));
        meshForearm.applyTransform(baseMatrix);
        createSegment(4, baseMatrix, arm.Arm.at(4-1).segmentMatrix);

        // Generate sphere (wristJoint), with origin to match top of forearm
        // Should be translated to match (0,0,(0.70+2.80+2.25))
        JointGeometry wristJoint = makeJoint(0.17f);
        Mesh meshWristJoint(wristJoint.vertices,
                  wristJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshWristJoint.applyTransform(baseMatrix);
        createSegment(5, baseMatrix, arm.Arm.at(5-1).segmentMatrix);

        // Generate Rectangle (hand), with origin to match top of wristJoint/Forearm
        // Should be translated to match (0,0,(0.70+2.80+2.25)), 0.17
        RectangleGeometry hand = makeRectangle(0.41f, 0.18f, 0.34f);
        Mesh meshHand(hand.vertices,
                  hand.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 0.41f, 0.0f));
        baseMatrix = glm::rotate(baseMatrix, 
                                glm::radians(90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        meshHand.applyTransform(baseMatrix);
        createSegment(6, baseMatrix, arm.Arm.at(6-1).segmentMatrix);

        // Generate cylinders (stem1 and stem2), 0.58 / 0.045
        // Should have its own base translated to match end of hand (0,0,(0.70+2.80+2.25+0.82))
        CylinderGeometry stem1 = makeCylinder(0.58f, 0.045f, false);
        Mesh meshStem1(stem1.vertices,
                  stem1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        baseMatrix = glm::rotate(baseMatrix, 
                                glm::radians(-90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 1.0f, -0.15f));
        meshStem1.applyTransform(baseMatrix);
        createSegment(7, baseMatrix, arm.Arm.at(7-1).segmentMatrix);

        // Generate Spheres (fingerJoints1 and fingerJoints2), 0.58 / 0.045
        // Should have its base tranlated to match the end of stems (0,0,(0.70+2.80+2.25+0.82+0.58))
        JointGeometry fingerJoint1 = makeJoint(0.06f);
        Mesh meshFingerJoint1(fingerJoint1.vertices,
                  fingerJoint1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshFingerJoint1.applyTransform(baseMatrix);
        createSegment(8, baseMatrix, arm.Arm.at(8-1).segmentMatrix);

        // Generate Rectangles (finger1 and finger2), 0.06
        // Base should be translated to match end of stems (0,0,(0.70+2.80+2.25+0.82+0.58))
        RectangleGeometry finger1 = makeRectangle(0.30f, 0.085f, 0.07f);
        Mesh meshFinger1(finger1.vertices,
                  finger1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 0.30f, 0.0f));
        baseMatrix = glm::rotate(baseMatrix, 
                                glm::radians(90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        meshFinger1.applyTransform(baseMatrix);
        createSegment(9, baseMatrix, arm.Arm.at(9-1).segmentMatrix);


        CylinderGeometry stem2 = makeCylinder(0.58f, 0.045f, false);
        Mesh meshStem2(stem2.vertices,
                  stem2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        baseMatrix = glm::rotate(baseMatrix, 
                                glm::radians(-90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, -0.3f, 0.5f));
        meshStem2.applyTransform(baseMatrix);
        createSegment(10, baseMatrix, arm.Arm.at(7-1).segmentMatrix);

        JointGeometry fingerJoint2 = makeJoint(0.06f);
        Mesh meshFingerJoint2(fingerJoint2.vertices,
                  fingerJoint2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        meshFingerJoint2.applyTransform(baseMatrix);
        createSegment(11, baseMatrix, arm.Arm.at(8-1).segmentMatrix);

        RectangleGeometry finger2 = makeRectangle(0.30f, 0.085f, 0.07f);
        Mesh meshFinger2(finger2.vertices,
                  finger2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        baseMatrix = glm::translate(baseMatrix, glm::vec3(0.0f, 0.30f, 0.0f));
        baseMatrix = glm::rotate(baseMatrix, 
                                glm::radians(90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        meshFinger2.applyTransform(baseMatrix);
        createSegment(12, baseMatrix, arm.Arm.at(9-1).segmentMatrix);

        // Anytime a parent component moves, the children components that are effected should be
        // updated to ensure there positions are still correct         

        constexpr float fieldOfView = glm::radians(45.0f);
        constexpr float cubeRadius = 8.f;
        const glm::vec3 cameraTarget(0.0f, 5.0f, 0.0f);
        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);


        while (window.isOpen()) {

            while (const std::optional event = window.pollEvent()) {
                
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                } 

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
    
                    // Base Roll
                    if (keyPressed->scancode == sf::Keyboard::Scan::A) {
                        
                        // The resuting transformed matrix from base
                        glm::mat4 baseTransform = meshBase.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                        glm::radians(-2.0f),
                                                                        glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));

                        meshUpperArm.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                        glm::radians(-2.0f),
                                                                        glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
                        
                        meshElbowJoint.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                glm::radians(-2.0f),
                                                                glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));

                        meshForearm.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                    glm::radians(-2.0f),
                                                                    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
                        
                        meshHand.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                    glm::radians(-2.0f),
                                                                    glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))));
                                                                

                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::D) {
                        // Rotate Right
                        // The resuting transformed matrix from base
                        glm::mat4 baseTransform = meshBase.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                        glm::radians(2.0f),
                                                                        glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));

                        baseTransform = meshUpperArm.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                        glm::radians(2.0f),
                                                                        glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
                        
                        meshElbowJoint.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                glm::radians(2.0f),
                                                                glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));

                        baseTransform = meshForearm.applyTransform(glm::rotate(glm::mat4(1.0f),
                                                                    glm::radians(2.0f),
                                                                    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
                        
                    }

                    // Shoulder Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::W) {
                        // Up
                        
                        // Upper Arm
                        glm::mat4 shoulderTransform = glm::mat4(1.0f);
                        shoulderTransform = glm::translate(shoulderTransform, glm::vec3(0.0f, -2.80f, 0.0f));
                        shoulderTransform = glm::rotate(shoulderTransform,
                                                        glm::radians(-2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        shoulderTransform = glm::translate(shoulderTransform, glm::vec3(0.0f, 2.80f, 0.0f));
                        meshUpperArm.applyTransform(shoulderTransform);

                        // Elbow Joint
                        meshElbowJoint.applyTransform(shoulderTransform);
                        // Forearm
                       // glm::mat4 forearmTransform = glm::mat4(1.0f);
                        glm::mat4 forearmTransform = glm::mat4(1.0f);
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (-2.25f - 2.80f), 0.0f));
                        forearmTransform = glm::rotate(forearmTransform,
                                                        glm::radians(-2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (2.25f + 2.80f), 0.0f));
                        meshForearm.applyTransform(forearmTransform);

                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::S) {
                        // Down
                        glm::mat4 shoulderTransform = glm::mat4(1.0f);
                        shoulderTransform = glm::translate(shoulderTransform, glm::vec3(0.0f, -2.80f, 0.0f));
                        shoulderTransform = glm::rotate(shoulderTransform,
                                                        glm::radians(2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        shoulderTransform = glm::translate(shoulderTransform, glm::vec3(0.0f, 2.80f, 0.0f));
                        meshUpperArm.applyTransform(shoulderTransform);

                        meshElbowJoint.applyTransform(shoulderTransform);

                        glm::mat4 forearmTransform = glm::mat4(1.0f);
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (-2.25f - 2.80f), 0.0f));
                        forearmTransform = glm::rotate(forearmTransform,
                                                        glm::radians(2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (2.25f + 2.80f), 0.0f));
                        meshForearm.applyTransform(forearmTransform);
                    }

                    // Elbow Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::R) {
                        // Up Relative to Forearm
                        glm::mat4 forearmTransform = glm::mat4(1.0f);
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (-2.25f), 0.0f));
                        forearmTransform = glm::rotate(forearmTransform,
                                                        glm::radians(-2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (2.25), 0.0f));
                        meshForearm.applyTransform(forearmTransform);
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::F) {
                        // Down Relative to Forearm

                        glm::mat4 forearmTransform = glm::mat4(1.0f);
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (-2.25f), 0.0f));
                        forearmTransform = glm::rotate(forearmTransform,
                                                        glm::radians(2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        forearmTransform = glm::translate(forearmTransform, glm::vec3(0.0f, (2.25), 0.0f));
                        meshForearm.applyTransform(forearmTransform);
                    }

                    // Wrist Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::T) {
                        // Rotate hand up
                        glm::mat4 handTransform = glm::mat4(1.0f);
                        handTransform = glm::rotate(handTransform,
                                                        glm::radians(2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        meshHand.applyTransform(handTransform);
                        
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::G) {
                        // Rotate Hand down
                        glm::mat4 handTransform = glm::mat4(1.0f);
                        handTransform = glm::rotate(handTransform,
                                                        glm::radians(-2.0f),
                                                        glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
                        meshHand.applyTransform(handTransform);
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

                        meshBase.setModel(arm.Arm.at(0).originalMatrix);

                        meshShoulderJoint.setModel(arm.Arm.at(1).originalMatrix);
                        meshUpperArm.setModel(arm.Arm.at(2).originalMatrix);

                        meshElbowJoint.setModel(arm.Arm.at(3).originalMatrix);
                        meshForearm.setModel(arm.Arm.at(4).originalMatrix);

                        meshWristJoint.setModel(arm.Arm.at(5).originalMatrix);
                        meshHand.setModel(arm.Arm.at(6).originalMatrix);

                        meshStem1.setModel(arm.Arm.at(7).originalMatrix);
                        meshFingerJoint1.setModel(arm.Arm.at(8).originalMatrix);
                        meshFinger1.setModel(arm.Arm.at(9).originalMatrix);

                        meshStem2.setModel(arm.Arm.at(10).originalMatrix);
                        meshFingerJoint2.setModel(arm.Arm.at(11).originalMatrix);
                        meshFinger2.setModel(arm.Arm.at(12).originalMatrix);
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
                                                                        10.0f,
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
            meshHand.draw(view, projection);

            meshStem1.draw(view, projection);
            meshFingerJoint1.draw(view, projection);
            meshFinger1.draw(view, projection);

            meshStem2.draw(view, projection);
            meshFingerJoint2.draw(view, projection);
            meshFinger2.draw(view, projection);

            window.display();
        }

    } catch (const std::exception& error) {
        std::cerr << "Application Failure: " << error.what() << std::endl;
        return 1;
    }

}