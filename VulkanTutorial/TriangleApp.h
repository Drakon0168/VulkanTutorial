#pragma once
#include "pch.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	//Returns true if all of the queue families have been set
	bool IsConplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class TriangleApp
{
public:
	//Initailizes the window and starts the main loop
	void Run();
private:
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const int MAX_FRAMES_IN_FLIGHT = 2;

	GLFWwindow* window;
	glm::mat4 matrix;
	glm::vec4 vec;

	VkInstance instance;
	
	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageView;
	std::vector<VkFramebuffer> swapChainFrameBuffers;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
	//Draws all of the objects to the screen
	void DrawFrame();
	//Creates the semaphores to manage async frame rendering
	void CreateSyncObjects();

	//Initialize the Vulkan application
	void CreateInstance();
	//Finds out the extensions that are required for the program to run
	std::vector<const char*> GetRequiredExtensions();

	//Setup the physical device
	void PickPhysicalDevice();
	//Finds the graphics queue family associated with this physical device
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	//Checks that a physical device supports the required extensions
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	//Scores devices based on suitability in order to pick the best device
	int RateDevice(VkPhysicalDevice device);
	//Checks whether the device is capable of running the program
	bool IsDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceFeatures deviceFeatures);

	//Setup the logical device that interfaces with the physical device
	void CreateLogicalDevice();

	//Creates a surface for Vulkan to render to
	void CreateSurface();

	//Creates the swap chain
	void CreateSwapChain();
	//Creates the image views to be used by the swap chain
	void CreateImageViews();
	//Creates the frame buffers to be used by the swap chain
	void CreateFrameBuffers();
	//Populates the swap chain support struct
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	//Chooses one of the available swap chain formats
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	//Chooses one of the available swap chain present modes
	VkPresentModeKHR ChooseSwapSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	//Chooses the size of the frames in the swap chain
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

	//Creates the graphics pipeline
	void CreateGraphicsPipeline();
	//Creates the Render Pass
	void CreateRenderPass();
	//Creates the Vulkan shader from the shader data
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	//Creates the Command Pool
	void CreateCommandPool();
	//Creates the Command Buffers
	void CreateCommandBuffers();

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

	static std::vector<char> ReadFile(const std::string& filePath);
};