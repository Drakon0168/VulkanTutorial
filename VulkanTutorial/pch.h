#ifndef PCH_H
#define PCH_H

//GLFW Includes
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//GLM Includes
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <optional>

#endif //PCH_H