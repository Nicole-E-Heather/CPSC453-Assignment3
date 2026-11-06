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

#define MAX_ANGLE_SHOULDER 60
#define MAX_ANGLE_ELBOW 80
#define MAX_ANGLE_WRIST_UP 50
#define MAX_ANGLE_WRIST_DOWN 90
#define MAX_ANGLE_FINGERS_CLOSE 15
#define MAX_ANGLE_FINGERS_OPEN 30

// Each segment stores its matrices and geoetry to make 
// subsequent transformations during runtime easier
struct Segment {

    unsigned int partIndex; // indices to keep track just in case

    glm::mat4 segmentMatrix; // any shape
    glm::mat4 originalMatrix; // the matrix constructed at first segment generation
    // original is never modified unlike segment, used when arm is reset
};

// Store in a vector for easy access
struct RobotArm {
    std::vector<Segment> Arm;
};

float angleBase = 0.0f; // for base only
float angleShoulder = 0.0f; // for both shoulder joint and upper arm
float angleElbow = 0.0f; // for both elbow joint and forearm
float angleWrist = 90.0f; // for wrist joint and hand
float angleFingers = 0.0f; // for finger joints and fingers

RobotArm arm; // global for easy access



void createSegment(unsigned int partIndex, glm::mat4 segment) {

    Segment newSegment = {
        partIndex,
        segment,
        segment
    };

    arm.Arm.push_back(newSegment);
}

int main(int argc, char* argv[]) {

    // Code based off of repository examples

    try {


        // Setup code taken directly from starter code
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
        untextured.shininess = 100.0f;
    
        // Generation of Meshes is based heavily on startercode

        // Transform applied to meshes here are not strictly necessary, mostly used for
        // setup and geometry testing.

        // Start by generating cylinder (base), centered at origin (0,0,0) from the 

        // This matrix is the "world/base" matrix in which all starting positions should be applied to
        CylinderGeometry base = makeCylinder(1.10f, 0.70f, true);
        Mesh meshBase(base.vertices,
                  base.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  textured);
        glm::mat4 baseMatrix = glm::mat4(1.0f);
        meshBase.setTransform(baseMatrix);
        createSegment(0, baseMatrix);
        
        // Generate sphere (shoulderJoint), with origin positioned at the top center of the base
        JointGeometry shoulderJoint = makeJoint(0.24f);
        Mesh meshShoulderJoint(shoulderJoint.vertices,
                  shoulderJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 shoulderMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.10f, 0.f));
        meshShoulderJoint.setTransform(baseMatrix * shoulderMatrix);

        createSegment(1, baseMatrix * shoulderMatrix);

        // Generate cylinder (upperArm), with origin set to position on top of shoulder joint
        CylinderGeometry upperArm = makeCylinder(2.80f, 0.22f, false);
        Mesh meshUpperArm(upperArm.vertices,
                  upperArm.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_arm_linkage.png",
                  textured);
        meshUpperArm.setTransform(baseMatrix * shoulderMatrix);

        createSegment(2, baseMatrix * shoulderMatrix);

        // Generate sphere (elbowJoint), with origin set to top of upper arm
        JointGeometry elbowJoint = makeJoint(0.20f);
        Mesh meshElbowJoint(elbowJoint.vertices,
                  elbowJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 elbowMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.80f, 0.0f));
        meshElbowJoint.setTransform(baseMatrix * shoulderMatrix * elbowMatrix);

        createSegment(3, baseMatrix * shoulderMatrix * elbowMatrix);

        // Generate cylinder (forearm), with origin top of elbowJoint/upperArm
        CylinderGeometry forearm = makeCylinder(2.25f, 0.17f, false);
        Mesh meshForearm(forearm.vertices,
                  forearm.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_arm_linkage.png",
                  textured);
        meshForearm.setTransform(baseMatrix * shoulderMatrix * elbowMatrix);

        createSegment(4, baseMatrix * shoulderMatrix * elbowMatrix);

        // Generate sphere (wristJoint), with origin to match top of forearm
        JointGeometry wristJoint = makeJoint(0.17f);
        Mesh meshWristJoint(wristJoint.vertices,
                  wristJoint.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 wristMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.25f, 0.0f));

        meshWristJoint.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix);
        createSegment(5, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix);

        // Generate Rectangle (hand), with origin to match top of wristJoint/Forearm
        RectangleGeometry hand = makeRectangle(0.41f, 0.18f, 0.34f);
        Mesh meshHand(hand.vertices,
                  hand.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 handMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.41f, 0.0f))
                                * glm::rotate(glm::mat4(1.0f), 
                                            glm::radians(90.0f),
                                            glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        meshHand.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix * handMatrix);
        createSegment(6, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix * handMatrix);

        // Generate cylinders (stem1 and stem2), 0.58 / 0.045
        CylinderGeometry stem1 = makeCylinder(0.58f, 0.045f, false);
        Mesh meshStem1(stem1.vertices,
                  stem1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 stem1Matrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)))
                                    * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.41f, -0.15f));
        meshStem1.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix * handMatrix * stem1Matrix);

        createSegment(7, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix * handMatrix * stem1Matrix);

        // Generate Spheres (fingerJoints1 and fingerJoints2), 0.58 / 0.045
        JointGeometry fingerJoint1 = makeJoint(0.06f);
        Mesh meshFingerJoint1(fingerJoint1.vertices,
                  fingerJoint1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 fingerJoint1Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.58f, 0.0f));
        meshFingerJoint1.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                        * handMatrix * stem1Matrix * fingerJoint1Matrix);

        createSegment(8, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                        * handMatrix * stem1Matrix * fingerJoint1Matrix);

        // Generate Rectangles (finger1 and finger2), 0.06
        RectangleGeometry finger1 = makeRectangle(0.30f, 0.085f, 0.07f);
        Mesh meshFinger1(finger1.vertices,
                  finger1.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 finger1Matrix = glm::rotate(glm::mat4(1.0f), 
                                glm::radians(90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        meshFinger1.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                        * handMatrix * stem1Matrix * fingerJoint1Matrix * finger1Matrix);

        createSegment(9, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                        * handMatrix * stem1Matrix * fingerJoint1Matrix * finger1Matrix);


        CylinderGeometry stem2 = makeCylinder(0.58f, 0.045f, false);
        Mesh meshStem2(stem2.vertices,
                  stem2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 stem2Matrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)))
                                * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.41f, 0.15f));
        meshStem2.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                 * handMatrix * stem2Matrix);

        createSegment(10, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                 * handMatrix * stem2Matrix);

        JointGeometry fingerJoint2 = makeJoint(0.06f);
        Mesh meshFingerJoint2(fingerJoint2.vertices,
                  fingerJoint2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 fingerJoint2Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.58f, 0.0f));
        meshFingerJoint2.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                 * handMatrix * stem2Matrix * fingerJoint2Matrix);

        createSegment(11, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                 * handMatrix * stem2Matrix * fingerJoint2Matrix);

        RectangleGeometry finger2 = makeRectangle(0.30f, 0.085f, 0.07f);
        Mesh meshFinger2(finger2.vertices,
                  finger2.indices,
                  (shaderDirectory / "mesh.vert").string(),
                  (shaderDirectory / "mesh.frag").string(),
                  "assets/texture_base_top.png",
                  untextured);
        glm::mat4 finger2Matrix = glm::rotate(glm::mat4(1.0f), 
                                glm::radians(90.0f),
                                glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        meshFinger2.setTransform(baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                 * handMatrix * stem2Matrix * fingerJoint2Matrix * finger2Matrix);

        createSegment(12, baseMatrix * shoulderMatrix * elbowMatrix * wristMatrix 
                                 * handMatrix * stem2Matrix * fingerJoint2Matrix * finger2Matrix);

        // Anytime a parent component moves, the children components that are effected should be
        // updated to ensure there positions are still correct   
        
        // Taken directly from starter code with value differences
        constexpr float fieldOfView = glm::radians(30.0f);
        constexpr float cameraRadius = 5.0f;
        const glm::vec3 cameraTarget(0.0f, 5.0f, 0.0f);
        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);


        while (window.isOpen()) {

            while (const std::optional event = window.pollEvent()) {
                
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                } 

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
    
                    // Base Roll: Angle can be whatever, 360 degress is fair
                    if (keyPressed->scancode == sf::Keyboard::Scan::A) {
                        // Rotate Left
                        angleBase -= 1.0f;
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::D) {
                        // Rotate Right
                        angleBase += 1.0f;
                    }

                    // Shoulder Pitch: 
                    if (keyPressed->scancode == sf::Keyboard::Scan::W) {

                        // Up
                        if (angleShoulder <= -MAX_ANGLE_SHOULDER) {

                            std::cout << "Max Range for Shoulder Acheived." << std::endl;
                        }
                        else {
                            angleShoulder -= 1.0f;
                        }
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::S) {

                        // Down
                        if (angleShoulder >= MAX_ANGLE_SHOULDER) {

                            std::cout << "Max Range for Shoulder Acheived." << std::endl;
                        }
                        else {
                            angleShoulder += 1.0f;
                        }
                    }

                    // Elbow Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::R) {
                        // Up Relative to Forearm
                        if (angleElbow <= -MAX_ANGLE_ELBOW) {

                            std::cout << "Max Range for Elbow Acheived." << std::endl;
                        }
                        else {
                            angleElbow -= 1.0f;   
                        }
                    
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::F) {
                        // Down Relative to Forearm
                        if (angleElbow >= MAX_ANGLE_ELBOW) {

                            std::cout << "Max Range for Elbow Acheived." << std::endl;
                        }
                        else {
                            angleElbow += 1.0f;   
                        }
                    }

                    // Wrist Pitch
                    if (keyPressed->scancode == sf::Keyboard::Scan::T) {
                        // Rotate hand up
                        if (angleWrist <= MAX_ANGLE_WRIST_UP) {

                            std::cout << "Max range for Wrist Acheived." << std::endl;
                        }
                        else {
                            angleWrist -= 1.0f;
                        }
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::G) {
                        // Rotate Hand down
                        if (angleWrist >= MAX_ANGLE_WRIST_DOWN) {

                            std::cout << "Max range for Wrist Acheived." << std::endl;
                        }
                        else {
                            angleWrist += 1.0f;
                        }
                    }

                    // Gripper Claw
                    if (keyPressed->scancode == sf::Keyboard::Scan::O) {
                        // Open Claw
                        if (angleFingers <= -MAX_ANGLE_FINGERS_OPEN) {

                            std::cout << "Max range for Fingers Acheived." << std::endl;
                        }
                        else {
                            angleFingers -= 1.0f;
                        }
                    }
                    
                    if (keyPressed->scancode == sf::Keyboard::Scan::C) {
                        // Close Claw
                       // angleFingers += 1.0f;
                       if (angleFingers >= MAX_ANGLE_FINGERS_CLOSE) {

                            std::cout << "Max range for Fingers Acheived." << std::endl;
                       }
                        else {
                            angleFingers += 1.0f;
                        }
                    }

                    // Reset Pose
                    if (keyPressed->scancode == sf::Keyboard::Scan::Space) {

                        // Reset Object-to-world transform to original values
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

                        angleBase = 0.0f;
                        angleShoulder = 0.0f;
                        angleElbow = 0.0f;
                        angleWrist = 90.0f;
                        angleFingers = 0.0f;
                    }

                    //std::cout << angleShoulder << std::endl;
                    //std::cout << angleElbow << std::endl;
                    //std::cout << angleWrist << std::endl;
                    //std::cout << angleFingers << std::endl;
                }
            }

            // Insetad of applyingTransform, just set the model matrix to the newly built matrix
            // Makes tracking operations much simpler.

            // The "model" matrix is what takes the local shape coordinates and translated them to the world system
            // Different way of putting it is taking the Global Frame (origin/parent) and translating to local coordinates
            glm::mat4 modelBase = glm::rotate(glm::mat4(1.0f), glm::radians(angleBase), glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 modelShoulder = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.10f, 0.0f)) 
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(angleShoulder), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 modelElbow = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.80f, 0.0f)) 
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(angleElbow), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 modelWrist = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.25f, 0.0f))
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(angleWrist), glm::vec3(0.0f, 0.0f, 1.0f));
                                                   
            glm::mat4 modelHand = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.41f, 0.0f))
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(angleWrist), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 modelStem1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0, 0.20f))
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            
            // The fingers are rotated with input angles first to make placement on the hand simpler
            // Fingers always totated by 90 degrees to stay inline with the hand.
            glm::mat4 modelFinger1 = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(angleFingers), glm::vec3(1.0f, 0.0f, 0.0f))
                                                    * glm::mat4(1.0f), glm::vec3(0.0f, -0.32f, 0.0f))
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 modelStem2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0, -0.20f))
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 modelFinger2 = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(- angleFingers), glm::vec3(1.0f, 0.0f, 0.0f))
                                                    * glm::mat4(1.0f), glm::vec3(0.0f, -0.32f, 0.0f))
                                                    * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            meshBase.setTransform(modelBase); // Parent component already centered at the origin, only needs rotation

            // All children must accumulate the transforms of its parents.
            // Multiplied left to right
            meshShoulderJoint.setTransform(modelBase * modelShoulder);
            meshUpperArm.setTransform(modelBase * modelShoulder);

            meshElbowJoint.setTransform(modelBase * modelShoulder * modelElbow);
            meshForearm.setTransform(modelBase * modelShoulder * modelElbow);

            meshWristJoint.setTransform(modelBase * modelShoulder * modelElbow * modelWrist);
            meshHand.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand);

            meshStem1.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand * modelStem1);
            meshFingerJoint1.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand * modelStem1);
            meshFinger1.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand * modelStem1 * modelFinger1);

            meshStem2.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand * modelStem2);
            meshFingerJoint2.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand * modelStem2);
            meshFinger2.setTransform(modelBase * modelShoulder * modelElbow * modelWrist * modelHand * modelStem2 * modelFinger2);


            // Taken directly from starter code.
            const sf::Vector2u size = window.getSize();
            glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
            const float aspectRatio = size.x == 0 || size.y == 0
                ? 1.0f
                : static_cast<float>(size.x) / static_cast<float>(size.y);
            const float limitingAspect = std::min(aspectRatio, 1.0f);
            const float limitingHalfFov = std::atan(
                std::tan(fieldOfView * 0.5f) * limitingAspect);
            const float cameraDistance = cameraRadius  / std::sin(limitingHalfFov) * 1.1f;
            const glm::mat4 view = glm::lookAt(cameraTarget + glm::vec3(0.0f,
                                                                        10.0f,
                                                                        cameraDistance),
                                               cameraTarget,
                                               worldUp);
            const glm::mat4 projection = glm::perspective(fieldOfView,
                                                           aspectRatio,
                                                           0.1f,
                                                           cameraDistance + cameraRadius  * 2.0f);
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