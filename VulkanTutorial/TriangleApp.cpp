#include "pch.h"
#include "TriangleApp.h"

#pragma region Proxy Functions

//Create the Debug Utils Messenger from the Create Info that is set up in SetupDebugCallback()
VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
	) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, createInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//Destroy the Debug Utils Messenger when the window closes in Cleanup()
void DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator
	) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

#pragma endregion

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
	window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan Window", nullptr, nullptr);
}

void TriangleApp::InitVulkan()
{
	//Create application instance
	CreateInstance();

	//Set the debug callback function
	SetupDebugMessenger();

	//Pick the physical device to use
	PickPhysicalDevice();
}

void TriangleApp::Cleanup()
{
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	//Cleanup Vulkan resources
	vkDestroyInstance(instance, nullptr);

	//Cleanup GLFW resources
	glfwDestroyWindow(window);

	//Terminate the window
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
	uint32_t requiredExtensionsCount = static_cast<uint32_t>(requiredExtensions.size());

	/*
	//Print out the extensions that have been found and the extensions that are needed
	std::cout << "Found " << extensionCount << " extensions:" << std::endl;
	for (VkExtensionProperties e : extensions) {
		std::cout << "\t" << e.extensionName << std::endl;
	}

	std::cout << "\nRequired " << requiredExtensionsCount << " extensions:" << std::endl;
	for (uint32_t i = 0; i < requiredExtensionsCount; i++) {
		std::cout << "\t" << requiredExtensions[i] << std::endl;
	}
	*/

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
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	//Create the instance
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

void TriangleApp::SetupDebugMessenger()
{
	//Don't run if validation labels are disabled
	if (!enableValidationLayers) {
		return;
	}

	//Setup the CreateInfo
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	//Set the messenger
	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup messenger!");
	}
}

void TriangleApp::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU's with Vulkan support!");
	}

	//Get physical devices
	std::vector<VkPhysicalDevice> physicalDevices;
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

	//Add devices to sorted list based on suitability
	std::multimap<int, VkPhysicalDevice> deviceMap;

	for (auto device : physicalDevices) {
		int score = RateDevice(device);
		deviceMap.insert(std::make_pair(score, device));
	}

	//Check the key of the first element in the map
	if (deviceMap.rbegin()->first > 0) {
		//If the key is valid use this physical device
		physicalDevice = deviceMap.rbegin()->second;
	}
	else {
		throw std::exception("No suitable decive found!");
	}
}

int TriangleApp::RateDevice(VkPhysicalDevice device)
{
	//Get Device Properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//Get Device Features
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//Check for base specifications needed to run the program, if these are not met return 0
	if (!(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader)) {
		return 0;
	}

	return 1;
}

void TriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
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

VKAPI_ATTR VkBool32 VKAPI_CALL TriangleApp::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	//TODO: Figure out how to change text colors to make errors and warnings stand out more	
	//Methods for handling different types of messages unused for the moment so it is commented out
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		//Diagnostic Messages
		std::cout << "Diagnostic Message: " << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		//Informational Messages
		std::cout << "Informational Message: " << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		//Warnings
		std::cout << "Warning: " << std::endl;
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		//Errors
		std::cout << "Error: " << std::endl;
		break;
	}

	/*switch (messageType) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		//Unrelated events
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		//Violation of specifications, or possible mistake
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		//Performance could be improved
		break;
	}*/

	std::cerr << "\tValidation Layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

#pragma endregion