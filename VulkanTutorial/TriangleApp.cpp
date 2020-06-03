#include "pch.h"
#include "TriangleApp.h"

#pragma region Setup

void TriangleApp::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}

void TriangleApp::InitWindow()
{
	//Initialize GLFW
	glfwInit();

	//Prevent GLFW from loading OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Prevent the window from being resized
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Create the window
	window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan window", nullptr, nullptr);
}

void TriangleApp::InitVulkan()
{
	//Create application instance
	CreateInstance();
}

void TriangleApp::Cleanup()
{
	//Cleanup Vulkan resources
	vkDestroyInstance(instance, nullptr);

	//Cleanup GLFW resources
	glfwDestroyWindow(window);
	glfwTerminate();
}

#pragma endregion

#pragma region Main Loop

void TriangleApp::MainLoop()
{
	auto test = matrix * vec;

	//Loop until the window is closed
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

#pragma endregion

#pragma region Vulkan Setup

void TriangleApp::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("Requested validation layer is not available!");
	}

	/*
	Setup App Info
		Provides information about the application to the graphics drivers for better optimization
	*/
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Triangle App";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	/*
	Setup Instance Info
		Allows GLFW and Vulkan to interface
	*/
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Check for optional extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	//Find the number of required extensions
	std::vector<const char*> requiredExtensions = getRequiredExtensions();
	uint32_t requiredExtensionsCount = requiredExtensions.size();

	//Print out the extensions that have been found and the extensions that are needed
	std::cout << "Found " << extensionCount << " extensions:" << std::endl;
	for (VkExtensionProperties e : extensions) {
		std::cout << "\t" << e.extensionName << std::endl;
	}

	std::cout << "\nRequired " << requiredExtensionsCount << " extensions:" << std::endl;
	for (uint32_t i = 0; i < requiredExtensionsCount; i++) {
		std::cout << "\t" << requiredExtensions[i] << std::endl;
	}

	//Ensure that required extensions exist
	for (uint32_t i = 0; i < requiredExtensionsCount; i++) {
		bool foundExtension = false;

		for (uint32_t j = 0; j < extensionCount; j++) {
			if (strcmp(requiredExtensions[i], extensions[j].extensionName) == 0) {
				foundExtension = true;
				break;
			}
		}

		if (!foundExtension) {
			char message[256] = "Could not find required extension: ";
			strcat_s(message, sizeof(message), requiredExtensions[i]);

			throw std::runtime_error(message);
		}
	}

	//Add supported extensions to the CreateInfo struct
	createInfo.enabledExtensionCount = requiredExtensionsCount;
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;

	//Add validation layers if they are enabled
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	//Create the instance
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

bool TriangleApp::CheckValidationLayerSupport()
{
	//Find available layers
	uint32_t validationLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(validationLayerCount);
	vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

	//Check for requested layers in the available layer list
	for (const char* layer : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layer, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> TriangleApp::getRequiredExtensions()
{
	//Find extensions required by GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//If validation layers are enabled add Debug Utilities to required extension list
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

#pragma endregion