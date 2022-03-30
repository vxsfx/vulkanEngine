#include <vulkan/vulkan.h>
#include <string>
#include <stdexcept>
#include <vector>

#include <iostream>
#include <cstdlib>
#ifndef DEBUGH
#define DEBUGH


const std::vector<const char*> validationLayers = {
"VK_LAYER_KHRONOS_validation",
"VK_LAYER_LUNARG_monitor"
//"VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT"
};


//create Debug validation class/struct
//https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers

inline bool validationSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layer : validationLayers) {
		bool layerfound = false;
		for (const auto& properties : availableLayers) {
			if (strcmp(layer, properties.layerName) == 0) {
				layerfound = true;
				break;
			}
		}
		if (!layerfound)
		{
			printf(layer);
			printf(" layer not found\n\n");
			return false;
		};
	}

	return true;
}

inline std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//if (debug) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	//}

	return extensions;
}

static VkDebugUtilsMessengerEXT debugMessenger;
inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

inline void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

inline VkResult CreateDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
inline void setupDebugMessenger(VkInstance& inst) {
	//if (!debug) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(inst, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}
inline void DestroyDebugUtilsMessengerEXT(VkInstance& instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}





inline void countInt(std::string type, std::string file, int line){
    throw std::runtime_error("ERROR: no " + type + " " + "found in file: " + file + "at line: " + std::to_string(line));
}



inline void VulkanError(VkResult res, std::string file, int line){
    std::string err;
    switch(res){
        default:
            err = "specific no err";
            break;
    };

    throw std::runtime_error("ERROR: " + err + "found in file: " + file + "at line: " + std::to_string(line));
}
#endif

#define isValidGfx(res) if(res != VK_SUCCESS) VulkanError(res, __FILE__, __LINE__)
#define isValid(res) if (res != true) 
#define isValidCount(count, type) if (count < 1) countInt(type, __FILE__, __LINE__)