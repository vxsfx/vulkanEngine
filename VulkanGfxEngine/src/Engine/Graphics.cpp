#include "Graphics.h"

#pragma region devices
void Graphics::createPhysicalDevice(){
    uint32_t deviceCount = 0;
    physDevice = VK_NULL_HANDLE;

	isValidGfx(vkEnumeratePhysicalDevices(Window::instance, &deviceCount, nullptr));

	isValidCount(deviceCount, "physcial device");

	std::vector<VkPhysicalDevice> devices(deviceCount);

	isValidGfx(vkEnumeratePhysicalDevices(Window::instance, &deviceCount, devices.data()));

	for (const auto& device : devices) {
		if (checkDevice(device)) {/////////////////////
			physDevice = device;
			break;
		}
	}
	if (physDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find gpu");
	}   
}

bool Graphics::checkDevice(VkPhysicalDevice device){
    VkPhysicalDeviceFeatures feats;
	vkGetPhysicalDeviceFeatures(device, &feats);


	indices = checkQueueSupport(device);
	bool extSupport = checkExtensionSupport(device);


	bool swapsGud = false;
	if (extSupport) {
		SwapChainSupportDetails swapSupport = checkSwapChainSupport(device);
		swapsGud = !swapSupport.formats.empty() && !swapSupport.presentMode.empty();
	}

	return indices.isComplete() && extSupport && swapsGud && feats.samplerAnisotropy;
}

Graphics::QueueIndices Graphics::checkQueueSupport(VkPhysicalDevice device){
    QueueIndices nindices;
    // Logic to find queue family indices to populate struct with

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	


	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport = false;
		//device = pdevice remove arg
		isValidGfx(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Window::surface, &presentSupport));
		if (presentSupport) {
			nindices.presentFamily = i; 
		}

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			nindices.graphicsFamily = i;
		}
		if (nindices.isComplete()) {
			break;
		}
		i++;
	}
	
	return nindices;

}

void Graphics::createLogicalDevice(){
    //QueueIndices indices = checkQueueSupport(physDevice);

    std::vector<VkDeviceQueueCreateInfo> qcis; //queueCreateInfo{};
	//std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	uint32_t uniqueQueueFamilies[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };//<- may have to be moved down to SWAP CHAIN

	float queuePriority = 1.0f;
	for (uint32_t family : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo qci{};
		qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		qci.queueFamilyIndex = family;
		qci.queueCount = 1;
		qci.pQueuePriorities = &queuePriority;
		qcis.push_back(qci);
	}



	VkPhysicalDeviceFeatures df{};//deviceFeatures{};
	df.samplerAnisotropy = VK_TRUE;


	VkDeviceCreateInfo ncreateInfo{};
	ncreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	ncreateInfo.pQueueCreateInfos = qcis.data();
	ncreateInfo.queueCreateInfoCount = static_cast<uint32_t>(qcis.size());
	ncreateInfo.pEnabledFeatures = &df;
	ncreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	ncreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	ncreateInfo.enabledLayerCount = 0;

	isValidGfx(vkCreateDevice(physDevice, &ncreateInfo, nullptr, &logiDevice));

	vkGetDeviceQueue(logiDevice, indices.graphicsFamily.value(), 0, &gfxQueue);
	vkGetDeviceQueue(logiDevice, indices.presentFamily.value(), 0, &presentQueue);
}

bool Graphics::checkExtensionSupport(VkPhysicalDevice device){
    uint32_t extCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExtensions.data());


	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

#pragma endregion




void Graphics::createCommandPool(){
    QueueIndices queueFamily = checkQueueSupport(physDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamily.graphicsFamily.value();

	isValidGfx(vkCreateCommandPool(logiDevice, &poolInfo, nullptr, &commandPool));
}

Graphics::SwapChainSupportDetails Graphics::checkSwapChainSupport(VkPhysicalDevice device){
    SwapChainSupportDetails details;
	isValidGfx(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Window::surface, &details.capabilities));
	
	uint32_t formatCount;
	
	isValidGfx(vkGetPhysicalDeviceSurfaceFormatsKHR(device, Window::surface, &formatCount, nullptr));

	if (formatCount != 0 ) {
		details.formats.resize(formatCount);
		isValidGfx(vkGetPhysicalDeviceSurfaceFormatsKHR(device, Window::surface, &formatCount, details.formats.data()));
	}

	uint32_t presentModeCount;
	isValidGfx(vkGetPhysicalDeviceSurfacePresentModesKHR(device, Window::surface, &presentModeCount, nullptr));

	if (presentModeCount != 0) {
		details.presentMode.resize(presentModeCount);
		isValidGfx(vkGetPhysicalDeviceSurfacePresentModesKHR(device, Window::surface, &presentModeCount, details.presentMode.data()));
	}

	return details;
}
VkSurfaceFormatKHR Graphics::chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats){
    for (const auto& format : availableFormats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB) {
			return format;
		}
	}
	return availableFormats[0];
}
VkPresentModeKHR Graphics::choosePresentMode(std::vector<VkPresentModeKHR> availableModes){
    for (const auto& mode : availableModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D Graphics::chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities){
if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(Window::wnd, &width, &height);
	
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(
			actualExtent.width, 
			capabilities.minImageExtent.width, 
			capabilities.maxImageExtent.width);
	
		actualExtent.height = std::clamp(
			actualExtent.width,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);
		return actualExtent;
	}
}

void Graphics::createSwapChain(){
    SwapChainSupportDetails swapSupport = checkSwapChainSupport(physDevice);
	surfaceFormat = chooseSurfaceFormat(swapSupport.formats);
	VkPresentModeKHR presentMode = choosePresentMode(swapSupport.presentMode);
	swapExtent = chooseSwapExtent(swapSupport.capabilities);

	uint32_t imageCount = swapSupport.capabilities.minImageCount + 1;
	if (swapSupport.capabilities.maxImageCount > 0 && imageCount > swapSupport.capabilities.maxImageCount) {
		imageCount = swapSupport.capabilities.maxImageCount;
	}


    uint32_t uniqueQueueFamilies[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	
	VkSwapchainCreateInfoKHR sci{};
	sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	sci.surface = Window::surface;
	sci.minImageCount = imageCount;
	sci.imageFormat = surfaceFormat.format;
	sci.imageColorSpace = surfaceFormat.colorSpace;
	sci.imageExtent = swapExtent;
	sci.imageArrayLayers = 1;
	sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; //need to use layout VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

	if (indices.graphicsFamily != indices.presentFamily) {
		sci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		sci.queueFamilyIndexCount = 2;
		sci.pQueueFamilyIndices = uniqueQueueFamilies;
	}
	else {
		sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		sci.queueFamilyIndexCount = 0;
		sci.pQueueFamilyIndices = nullptr;
	}
	sci.preTransform = swapSupport.capabilities.currentTransform;
	sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	sci.presentMode = presentMode;
	sci.clipped = VK_TRUE;

	sci.oldSwapchain = VK_NULL_HANDLE;
	isValidGfx(vkCreateSwapchainKHR(logiDevice, &sci, nullptr, &swapChain));
}

//may need move to camera, WILL NEED TO MOVE TO CAMERA// TODO
void Graphics::createDescriptorPool(){
    std::array<VkDescriptorPoolSize, 2> poolSizes{};


	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;
	
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

	
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;//*2, number of gfxObjects???
	

	isValidGfx(vkCreateDescriptorPool(logiDevice, &poolInfo, nullptr, &descriptorPool));
}

void Graphics::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	fences.resize(MAX_FRAMES_IN_FLIGHT);
	imageFences.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semInfo{};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(logiDevice, &semInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(logiDevice, &semInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
			|| vkCreateFence(logiDevice, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed semaphores creation");
		}
	}

}

void Graphics::init(){
    deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

    createPhysicalDevice();
    createLogicalDevice();

    createCommandPool();
    createDescriptorPool();
    //add uninited camera first
    createSwapChain();

	createSyncObjects();
}

void Graphics::bindCamera(std::vector<VkImage>& images){
    uint32_t count;

    isValidGfx(vkGetSwapchainImagesKHR(logiDevice, swapChain, &count, nullptr));
    
    //override count given
    images.resize(count);
    vkGetSwapchainImagesKHR(logiDevice, swapChain, &count, images.data());
    swapImageFormat = surfaceFormat.format;
}


VkFormat Graphics::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features){
        for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physDevice, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	throw std::runtime_error("failed to find format ");
}

uint32_t Graphics::findMemoryType(uint32_t filter, VkMemoryPropertyFlags props) {
	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(physDevice, &memProp);

	for (uint32_t i = 0; i < memProp.memoryTypeCount; i++) {
		if (filter & (1 << i)  && (memProp.memoryTypes[i].propertyFlags & props) == props) {
			return i;
		}
	}

	throw std::runtime_error("failed mem type");
};



VkCommandBuffer Graphics::beginSingleTimeCommand(){
    VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	isValidGfx(vkAllocateCommandBuffers(logiDevice, &allocInfo, &commandBuffer));
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	isValidGfx(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	return commandBuffer;
};

void Graphics::endSingleTimeCommand(VkCommandBuffer& cmdBuffer){
    isValidGfx(vkEndCommandBuffer(cmdBuffer));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	isValidGfx(vkQueueSubmit(gfxQueue, 1, &submitInfo, VK_NULL_HANDLE));
	isValidGfx(vkQueueWaitIdle(gfxQueue));
	vkFreeCommandBuffers(logiDevice, commandPool, 1, &cmdBuffer);
};

void Graphics::destroy(){

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(logiDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logiDevice, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logiDevice, fences[i], nullptr);
    }


    vkDestroySwapchainKHR(logiDevice, swapChain, nullptr);
    vkDestroyDescriptorPool(logiDevice, descriptorPool, nullptr);
    vkDestroyCommandPool(logiDevice, commandPool, nullptr);

    vkDestroyDevice(logiDevice, nullptr);
}