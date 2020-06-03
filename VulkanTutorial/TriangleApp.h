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
	VkDebugUtilsMessengerEXT debugMessenger;
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

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
	//Setup the debug util messenger
	void SetupDebugMessenger();
	//Checks that all requested validation layers are supported
	bool CheckValidationLayerSupport();
	//Finds out the extensions that are required for the program to run
	std::vector<const char*> getRequiredExtensions();

	//The method that is called when Vulkan throws an error
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
		);
};