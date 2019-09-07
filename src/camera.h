#ifndef MOD_CAMERA
#define MOD_CAMERA
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

glm::quat setFrontDelta(glm::quat orientation, float deltaYaw, float deltaPitch, float deltaRoll, float delta);
glm::vec3 moveRelative(glm::vec3 position, glm::quat orientation, glm::vec3 offset);
glm::vec3 move(glm::vec3 position, glm::vec3 offset);
glm::mat4 renderView(glm::vec3 position, glm::quat orientation);

#endif 
