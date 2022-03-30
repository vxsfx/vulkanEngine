#include <vulkan/vulkan.h>
#include "../Graphics.h"
#include <iostream>

#ifndef BUFFER
#define BUFFER

namespace Buffer {
    inline void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height) {
        VkCommandBuffer cmdBuffer = Graphics::beginSingleTimeCommand();
        VkBufferImageCopy region{};

        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(cmdBuffer, buffer, image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &region
        );


        Graphics::endSingleTimeCommand(cmdBuffer);
    }

    inline bool create(VkBuffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage){
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;//verts in header
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //should return res
        return (vkCreateBuffer(Graphics::logiDevice, &bufferInfo, nullptr, &buffer) == VK_SUCCESS);//return as to allow line of where called
    }

    inline bool allocateAndBind(VkBuffer& buffer, VkDeviceMemory& memory, VkMemoryPropertyFlags props){
        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(Graphics::logiDevice, buffer, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = Graphics::findMemoryType(memReq.memoryTypeBits, props);

        return (vkAllocateMemory(Graphics::logiDevice, &allocInfo, nullptr, &memory)==VK_SUCCESS) && 
        (vkBindBufferMemory(Graphics::logiDevice, buffer, memory, 0) == VK_SUCCESS);
    }

    inline bool copy(VkBuffer& src, VkBuffer& dst, VkDeviceSize size){
        VkCommandBuffer cmdBuffer = Graphics::beginSingleTimeCommand();
	
	    VkBufferCopy copyRegion{};
	    copyRegion.size = size;
	    vkCmdCopyBuffer(cmdBuffer, src, dst, 1, &copyRegion);
	
	    Graphics::endSingleTimeCommand(cmdBuffer);
        return true;
    }

    inline bool createAllocateAndBind(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props) {
        bool a = false;
        bool b = false;
        a = create(buffer, size, usage);
        if (!a) { std::cout << "failed to create" << std::endl; }
        b = allocateAndBind(buffer, memory, props);
        if (!b) { std::cout << "failed to allocate and bind" << std::endl; }
        return (
            a && b
        );
    }
}
#endif