#pragma once
#define GLFW_INCLUDE_VULKAN
#include "external/glfw3.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

#include <array>
#include <vector>
#include <optional>

struct queueFamilyIndices_t {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	inline bool isComplete(void) {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct swapChainSupportDetails_t {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct vertex_t {
	glm::vec2 pos;
	
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(vertex_t);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(vertex_t, pos);
		return attributeDescriptions;
	}
};

const uint32_t barsCount = 256;
struct uniformBuffer_t {
	glm::vec4 heights[barsCount / 4];
};

struct renderer_t {
	const char* appName_c = "Audio visualizer";
	const uint32_t version_c = 01; // 01 = 0.1
	const std::vector<const char*> deviceExtensions_c = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const uint8_t maxFramesInFlight_c = 2;

	GLFWwindow* window;
	uint32_t winSizeX = 1200, winSizeY = 1000;
	bool framebuffResized = false;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<vertex_t> vertices = {
		{ {-1.0f, 0.4f} },
		{ {-1.0f + 2.0f / barsCount, 0.4f} },
		{ {-1.0f + 2.0f / barsCount, -1.0f} },
		{ {-1.0f, -1.0f} }
	};
	std::vector<uint32_t> indices = { 0,1,2,2,3,0 };
	std::vector<float> heights;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;

	void initRenderer(void);
	
	static void framebuffResizeCallback(GLFWwindow*, int32_t, int32_t);
	void initWindow(void);
	void initVk(void);
	void createWinSurface(void);

	queueFamilyIndices_t findQueueFamilies(VkPhysicalDevice _device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice _device);
	swapChainSupportDetails_t querySwapChainSupport(VkPhysicalDevice _device);
	bool isDeviceSuitable(VkPhysicalDevice _device);
	void pickPhysicalDevice(void);
	void createLogicalDevice(void);
	void createSwapChain(void);

	void createVkInstance(void);
	void setupDebugMessenger(void);
	void createImageViews(void);
	void createRenderPass(void);
	void createDescriptorSetLayout(void);
	void createGraphicsPipeline(void);
	void createFramebuffers(void);
	void createCommandPool(void);
	VkCommandBuffer beginSingleTimeCommands(void);
	void endSingleTimeCommands(VkCommandBuffer);
	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
	void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
	void createVertexBuffer(void);
	void createIndexBuffer(void);
	void createUniformBuffers(void);
	void createDescriptorPool(void);
	void createDescriptorSets(void);
	void createCommandBuffers(void);
	void createSyncObjects(void);
	
	void recreateSwapChain(void);
	void recordCommandBuffer(VkCommandBuffer, uint32_t);

	void updateUBO(void);
	void drawFrame(void);

	void cleanupSwapChain(void);
	void cleanup(void);
};