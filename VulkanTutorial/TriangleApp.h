#pragma once
#include "pch.h"

#include <stdexcept>
#include <functional>
#include <cstdlib>

class TriangleApp
{
public:
	//Initailizes the window and starts the main loop
	void Run();
private:
	const int windowWidth = 800;
	const int windowHeight = 600;

	GLFWwindow* window;
	glm::mat4 matrix;
	glm::vec4 vec;

	VkInstance instance;
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	//Initialize GLFW resources and create the window
	void InitWindow();
	//Initialize Vulkan resources
	void InitVulkan();
	//Cleans up GLFW and Vulkan resources called after the window is closed
	void Cleanup();
	//Called every frame and used to update objects on the screen
	void MainLoop();

	//Initialize the Vulkan application
	void CreateInstance();
	//Checks that all requested validation layers are supported
	bool CheckValidationLayerSupport();
};