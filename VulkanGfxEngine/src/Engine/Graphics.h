#include <vulkan/vulkan.h>
#include "Window.hpp"

#include <vector>
#include <optional>
#include <array>
#include <set>
#include <string>
#include <algorithm>

#ifndef GRAPHICSENG
#define GRAPHICSENG

#define MAX_FRAMES_IN_FLIGHT 2

class Graphics {
public:
    inline static uint32_t currentFrame;

    struct QueueIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentMode;
    };

    static void init();
    
    inline static QueueIndices indices;

    #pragma region Devices
    inline static VkPhysicalDevice physDevice;
    inline static VkDevice logiDevice;

    inline static std::vector<const char*> deviceExtensions;

    static void createPhysicalDevice();
    static void createLogicalDevice();
    static void chooseDevice();
    static bool checkDevice(VkPhysicalDevice device);
    
    //support
    static bool checkExtensionSupport(VkPhysicalDevice device);
    static QueueIndices checkQueueSupport(VkPhysicalDevice device);
    #pragma endregion


    inline static VkQueue gfxQueue;


    #pragma region Display
    //static VkSurfaceKHR surface;

    inline static VkExtent2D swapExtent;

    inline static VkSwapchainKHR swapChain;
    inline static VkSurfaceFormatKHR surfaceFormat;
    inline static VkFormat swapImageFormat;


    static void createSwapChain();
    static void bindCamera(std::vector<VkImage>& images);

    static SwapChainSupportDetails checkSwapChainSupport(VkPhysicalDevice device);    
    static VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
    static VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> availableFodes);
    static VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities);
    #pragma endregion


    static VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    
    static uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags props);



    inline static VkCommandPool commandPool;
    static void createCommandPool();

    inline static VkDescriptorPool descriptorPool;
    static void createDescriptorPool();

    static VkCommandBuffer beginSingleTimeCommand();
    static void endSingleTimeCommand(VkCommandBuffer& cmdBuffer);


    //mya move to camera
    inline static std::vector<VkSemaphore> imageAvailableSemaphores;
	inline static std::vector<VkSemaphore> renderFinishedSemaphores;
	//in flight objects
	inline static std::vector<VkFence> fences;
	inline static std::vector<VkFence> imageFences;
    static void createSyncObjects();

    

    inline static VkQueue presentQueue;
    static void present();


    static void destroy();
};

#endif