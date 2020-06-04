#pragma once
#include "pch.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	//Returns true if all of the queue families have been set
	bool IsConplete() {
		return graphicsFamily.has_value();
	}
};

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
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
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
	//Finds out the extensions that are required for the program to run
	std::vector<const char*> GetRequiredExtensions();

	//Setup the physical device
	void PickPhysicalDevice();
	//Finds the graphics queue family associated with this physical device
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	//Scores devices based on suitability in order to pick the best device
	int RateDevice(VkPhysicalDevice device);

	//Setup the logical device that interfaces with the physical device
	void CreateLogicalDevice();
	
	//Setup the debug util messenger
	void SetupDebugMessenger();
	//Setup a Debug Util Messenger CreateInfo
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	//Checks that all requested validation layers are supported
	bool CheckValidationLayerSupport();

	//The method that is called when Vulkan throws an error
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
		);
};