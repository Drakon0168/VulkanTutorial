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
	if (enableValidationLayers) {
		std::cout << "Setting up Window . . ." << std::endl;
	}
	
	InitWindow();

	if (enableValidationLayers) {
		std::cout << "Setting up Vulkan . . ." << std::endl;
	}

	InitVulkan();

	if (enableValidationLayers) {
		std::cout << "Finished Setup" << std::endl;
	}

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
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Window", nullptr, nullptr);
}

void TriangleApp::InitVulkan()
{
	//Create application instance
	CreateInstance();

	//Set the debug callback function
	SetupDebugMessenger();

	//Setup the rendering surface
	CreateSurface();

	//Pick the physical device to use
	PickPhysicalDevice();

	//Create the logical device
	CreateLogicalDevice();

	//Create the swap chain
	CreateSwapChain();

	//Create the image views
	CreateImageViews();

	//Create the render pass
	CreateRenderPass();

	//Create the graphics pipeline
	CreateGraphicsPipeline();

	//Create the frame buffers
	CreateFrameBuffers();

	//Create the command pool
	CreateCommandPool();

	//Create the Command Buffers
	CreateCommandBuffers();
}

void TriangleApp::Cleanup()
{
	//Destroy Command Pool
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	//Destroy Frame Buffers
	for (auto frameBuffer : swapChainFrameBuffers) {
		vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);
	}

	//Destroy the graphics pipeline
	vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);

	//Destroy the Pipeline Layout
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);

	//Destroy Render Pass
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	//Destroy Image Views
	for (VkImageView view : swapChainImageView) {
		vkDestroyImageView(logicalDevice, view, nullptr);
	}

	//Destroy Swap Chain
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

	//Destroy Logical Device
	vkDestroyDevice(logicalDevice, nullptr);

	//Destroy Debug Utils Messenger if it was created
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	//Destroy the surface
	vkDestroySurfaceKHR(instance, surface, nullptr);

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
	std::vector<const char*> requiredExtensions = GetRequiredExtensions();
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

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
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

std::vector<const char*> TriangleApp::GetRequiredExtensions()
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

#pragma region Physical Device Management

void TriangleApp::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU's with Vulkan support!");
	}

	//Get physical devices
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
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
		throw std::runtime_error("No suitable device found!");
	}
}

QueueFamilyIndices TriangleApp::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = {};

	//Find the queue families
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	//Find a queue that supports graphics operations
	int i = 0;

	for (const auto queueFamily : queueFamilies) {
		//Check for Graphics support
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		//Check for present support
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		//Exit the loop if we've found all of the necessary queue families
		if (indices.IsConplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool TriangleApp::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	//Ensure that all required extensions are on the available extensions list
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); //Make a copy of required extensions list
	for (auto extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName); //If the current extension is on the copied list, erase it
	}

	return requiredExtensions.empty(); //If the copied list is empty all required extensions have been found
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
	if (!IsDeviceSuitable(device, deviceFeatures)) {
		return 0;
	}

	//Set a score based on optional features, initial score of 1 because it meets requirements
	int score = 1;

	/*
	//Output device scoring information
	std::cout << deviceProperties.deviceName << " Score:" << std::endl;
	std::cout << "\tMeets Requirements: 1" << std::endl;
	std::cout << "\tDedicated Graphics Card: " << (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0) << std::endl;
	std::cout << "\tMax Image Dimensions 2D: " << deviceProperties.limits.maxImageDimension2D << std::endl;
	*/

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	return score;
}

bool TriangleApp::IsDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceFeatures deviceFeatures)
{
	//Device does not support geometry shaders
	if (!deviceFeatures.geometryShader) {
		return false;
	}

	//Device cannot process graphics commands
	QueueFamilyIndices queueFamilies = FindQueueFamilies(device);
	if (!queueFamilies.IsConplete()) {
		return false;
	}

	//Make sure that required extensions are supported
	if (!CheckDeviceExtensionSupport(device)) {
		return false;
	}

	//Make sure the swap chain is supported
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
	if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Logical Device Management

void TriangleApp::CreateLogicalDevice()
{
	//Setup Queue Families
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};
	float queuePriority = 1.0f;

	//Create queue create infos
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	//Set used device features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	//Setup Logical Device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else{
		createInfo.enabledLayerCount = 0;
	}

	//Create the Logical Device
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	//Set the queues
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

#pragma endregion

#pragma region Surface Management

void TriangleApp::CreateSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

#pragma endregion

#pragma region Swap Chain Management

void TriangleApp::CreateSwapChain()
{
	SwapChainSupportDetails details = QuerySwapChainSupport(physicalDevice);

	//Setup swap chain
	VkExtent2D extent = ChooseSwapExtent(details.capabilities);
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
	VkPresentModeKHR presentMode = ChooseSwapSurfacePresentMode(details.presentModes);

	//Set chain length
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;
	}

	//Setup Swap Chain
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//	-Setup Queue Families
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilies[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilies;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}
}

void TriangleApp::CreateImageViews()
{
	swapChainImageView.resize(swapChainImages.size());

	//Create Image Views
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		//Setup Create Info
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;

		//Create the Image View
		if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageView[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Image Views");
		}
	}
}

void TriangleApp::CreateFrameBuffers()
{
	//Resize vector to have enough space
	swapChainFrameBuffers.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainFrameBuffers.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageView[i]
		};

		//Setup frame buffer create infos
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;
		createInfo.width = swapChainExtent.width;
		createInfo.height = swapChainExtent.height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &createInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Frame Buffer!");
		}
	}
}

SwapChainSupportDetails TriangleApp::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	//Get Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//Get Surface Formats
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//Get Surface Modes
	uint32_t modeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, nullptr);
	if (modeCount != 0) {
		details.presentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR TriangleApp::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//Check for a format with the preferred settings
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	//If none of the formats have the preferred settings return the first format
	return availableFormats[0];
}

VkPresentModeKHR TriangleApp::ChooseSwapSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//Check for a present mode with the preferred settings
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	//If none of the present modes have the preferred settings return FIFO which is guaranteed to be supported
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D TriangleApp::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	//If possible pick a resolution that best matches the window within the min and max bounds
	if (surfaceCapabilities.currentExtent.width == UINT32_MAX) {
		VkExtent2D actualExtent = { WINDOW_WIDTH, WINDOW_HEIGHT };
		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}

	return surfaceCapabilities.currentExtent;
}

#pragma endregion

#pragma region Graphics Pipeline Management

void TriangleApp::CreateGraphicsPipeline()
{
	//Read in shader code
	auto vertexShaderCode = ReadFile("shaders/vert.spv");
	auto fragmentShaderCode = ReadFile("shaders/frag.spv");

	//Create shader module
	VkShaderModule vertexShaderModule = CreateShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentShaderCode);

	//Setup shader stages
	VkPipelineShaderStageCreateInfo vertexStageCreateInfo = {};
	vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexShaderModule;
	vertexStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentStageCreateInfo = {};
	fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageCreateInfo.module = fragmentShaderModule;
	fragmentStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertexStageCreateInfo,
		fragmentStageCreateInfo
	};

	//Setup the Vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;

	//Setup the Input Assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	//Setup the Viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Setup the Scissor Rectangle
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	//Setup Viewport State
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	//Setup Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizerCreateInfo.depthBiasClamp = 0.0f;
	rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

	//Setup Multisampling
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleCreateInfo.minSampleShading = 0.0f;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	//Setup Color Blending
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachmentState;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	//Setup Dynamic states
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;

	//Setup Pipeline Layout
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	//Create the pipeline layout
	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Graphics Pipeline Layout!");
	}

	//Setup graphics pipeline create info
	VkGraphicsPipelineCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderStages;
	createInfo.pVertexInputState = &vertexInputCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizerCreateInfo;
	createInfo.pMultisampleState = &multisampleCreateInfo;
	createInfo.pDepthStencilState = nullptr;
	createInfo.pColorBlendState = &colorBlendCreateInfo;
	createInfo.pDynamicState = &dynamicStateCreateInfo;
	createInfo.layout = pipelineLayout;
	createInfo.renderPass = renderPass;
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	//Create graphics pipeline
	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Graphics Pipeline!");
	}

	//Cleanup shader modules
	vkDestroyShaderModule(logicalDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(logicalDevice, fragmentShaderModule, nullptr);
}

void TriangleApp::CreateRenderPass()
{
	//Setup Color Attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Setup Subpass
	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	
	//Setup Create Info
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpassDescription;

	//Create Render Pass
	if (vkCreateRenderPass(logicalDevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Render Pass!");
	}
}

VkShaderModule TriangleApp::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Shader Module!");
	}

	return shaderModule;
}

#pragma endregion

#pragma region Command Buffer Management

void TriangleApp::CreateCommandPool()
{
	//Find Queue families
	QueueFamilyIndices queueFamilies = FindQueueFamilies(physicalDevice);

	//Setup Command Pool
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
	createInfo.flags = 0;

	if (vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Command Pool!");
	}
}

void TriangleApp::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = (int32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice, &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate Command Buffers");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		//Setup command
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording Command Buffer!");
		}

		//Setup render pass
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.extent = swapChainExtent;
		
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		//Setup commands
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end Command Buffer!");
		}
	}
}

#pragma endregion

#pragma region Debug Management

void TriangleApp::SetupDebugMessenger()
{
	//Don't run if validation labels are disabled
	if (!enableValidationLayers) {
		return;
	}

	//Setup the CreateInfo
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	//Set the messenger
	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup messenger!");
	}
}

void TriangleApp::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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

#pragma region Helper Functions

std::vector<char> TriangleApp::ReadFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!");
	}

	//Allocate space for the contents of the file
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	//Read the contents of the file
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	//Close the file and return
	file.close();
	return buffer;
}

#pragma endregion