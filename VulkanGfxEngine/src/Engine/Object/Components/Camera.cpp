#include "Camera.h"

Camera::Camera(std::string giveName, int width, int height, int count) : Component(giveName){
    extent.width = width;
    extent.height = height;
    images.resize(count);

	allCameras.push_back(this);
};

void Camera::createImageViews(VkFormat format){
    imageFormat = format;
    imageViews.resize(images.size());
  
	
	for (size_t i = 0; i < images.size(); i++) {
        /*
        VkImageViewCreateInfo vci{};
        vci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vci.image = images[i];
        vci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vci.format = imageFormat;
        vci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        vci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        vci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        vci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        vci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vci.subresourceRange.baseMipLevel = 0;
        vci.subresourceRange.levelCount = 1;
        vci.subresourceRange.baseArrayLayer = 0;
        vci.subresourceRange.layerCount = 1;
        */
        //isValidGfx(vkCreateImageView(Graphics::logiDevice, &vci, nullptr, &imageViews[i]));
        Image::createView(&imageViews[i], &images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);//VK_ACCESS_
    }

    //format should be set
    depthFormat = Graphics::findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );

}

void Camera::createDepthResource(){
    Image::create(&depthImage, extent.width, extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    Image::bindMemory(depthImage, diMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	Image::createView(&depthView, &depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void Camera::createRenderPass(){
    VkAttachmentDescription colorAttachment{};
	colorAttachment.format = imageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//intital layout, need transfer layout
	colorAttachment.finalLayout =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	VkAttachmentReference colorAttRef{};
	colorAttRef.attachment = 0;
	colorAttRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	


	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = depthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttRef{};
	depthAttRef.attachment = 1;
	depthAttRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//sVkSubpassDescription subpasses[2];

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttRef;
	subpass.pDepthStencilAttachment = &depthAttRef;



	
	//////////////////maskss are here
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | 
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	std::array<VkAttachmentDescription, 2> atts = {colorAttachment, depthAttachment};////////////////////////////////////////
	VkRenderPassCreateInfo passInfo{};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passInfo.attachmentCount = static_cast<uint32_t>(atts.size());
	passInfo.pAttachments = atts.data();
	passInfo.subpassCount = 1;
	passInfo.pSubpasses = &subpass;
	passInfo.dependencyCount = 1;
	passInfo.pDependencies = &dependency;
	
	isValidGfx(vkCreateRenderPass(Graphics::logiDevice, &passInfo, nullptr, &renderPass));
}

void Camera::setProjection(float ratio, float fov, float near, float far)
{
	float PI = 3.142;
	float half_angle = tan(fov * PI / 360);

	projection = {
		{1 / (ratio * half_angle), 0.0f, 0.0f, 0.0f},
		{0.0f, -1 / half_angle, 0.0f, 0.0f,},
		{0.0f, 0.0f,  -(far + near) / (far - near), -1.f},
		{0.0f, 0.0f, -2 * far * near / (far - near), 0.0f}
	};
}


void Camera::lookAt(float3 towards)
{
	//+ cameraTransform->position
	view = mat4::lookAt(towards + cameraTransform->position, cameraTransform->position, {0.0f, 1.0f, 0.0f});
}

void Camera::createFrameBuffers(){
    frameBuffers.resize(imageViews.size());

	for (size_t i = 0; i < imageViews.size(); i++) {
		VkImageView attachements[] = {
			imageViews[i],
			depthView,
		};

		VkFramebufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		bufferInfo.renderPass = renderPass;
		bufferInfo.attachmentCount = 2;//attachments.size()///////////////////////////////////////////////
		bufferInfo.pAttachments = attachements;
		bufferInfo.width = extent.width;
		bufferInfo.height = extent.height;
		bufferInfo.layers = 1;


		isValidGfx(vkCreateFramebuffer(Graphics::logiDevice, &bufferInfo, nullptr, &frameBuffers[i]));
	}
}

//getDescriptorImageInfos will be in texture aswell//for texture views
std::vector<VkDescriptorImageInfo> Camera::getDescriptorImageInfos(){
    std::vector<VkDescriptorImageInfo> imageInfos;
    for(auto& imageView : imageViews){
        VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;  
		imageInfo.imageView = imageView;//textureView
		//imageInfo.sampler = texSampler; //sampler
        imageInfos.push_back(imageInfo);
    }
    return imageInfos;
}

void Camera::createCommandBuffers(){    
    commandBuffers.resize(frameBuffers.size());
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = Graphics::commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	isValidGfx(vkAllocateCommandBuffers(Graphics::logiDevice, &allocInfo, commandBuffers.data()));
	//main screen
	for (size_t i = 0; i < commandBuffers.size(); i++) {
		//commandBUFFERRECORDING
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		isValidGfx(vkBeginCommandBuffer(commandBuffers[i], & beginInfo));
	///////

		//VkClearValue clearColor = { {{0.f, 0.f, 0.f, 1.f}} };
		std::array<VkClearValue, 2> clearValues{};//order of attachments in ranfder pass
		clearValues[0].color = {0.f, 0.f, 0.f, 1.f};
		clearValues[1].depthStencil = { 1.0f, 0};

		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = renderPass;
		passInfo.framebuffer = frameBuffers[i];
		passInfo.renderArea.offset = { 0,0 };
		passInfo.renderArea.extent = extent;
		passInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		passInfo.pClearValues = clearValues.data();
		
		vkCmdBeginRenderPass(commandBuffers[i], &passInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		
        for(BaseObjectGfx* gfxObj : BaseObjectGfx::allGfx){
		    //SECONDARY CMDBUFFER   
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, gfxObj->pipeline);
            
            VkBuffer vertexBuffers[] = { gfxObj->vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffers[i], gfxObj->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, gfxObj->pipelineLayout, 0, 1, &gfxObj->descriptorSets[i], 0, nullptr);

            //vkCmdDraw(cmdBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(gfxObj->indices.size()),1, 0, 0,0);
            }
		//SECONDARY
		
		vkCmdEndRenderPass(commandBuffers[i]);
		isValidGfx(vkEndCommandBuffer(commandBuffers[i]));
	}
}

//sub cam draw, 
void Camera::draw(){
    isValidGfx(vkWaitForFences(Graphics::logiDevice, 1, &Graphics::fences[Graphics::currentFrame], VK_TRUE, UINT64_MAX));
	

	uint32_t imageIndex;
    //grab next index
	isValidGfx(vkAcquireNextImageKHR(Graphics::logiDevice, Graphics::swapChain, UINT64_MAX, Graphics::imageAvailableSemaphores[Graphics::currentFrame], VK_NULL_HANDLE, &imageIndex));


	if (Graphics::imageFences[imageIndex] != VK_NULL_HANDLE) {
		isValidGfx(vkWaitForFences(Graphics::logiDevice, 1, &Graphics::imageFences[imageIndex], VK_TRUE, UINT64_MAX));
	}
    // above for static use

    // below per camera

	Graphics::imageFences[imageIndex] = Graphics::fences[Graphics::currentFrame ];

	VkSemaphore waits[] = { Graphics::imageAvailableSemaphores[Graphics::currentFrame]};
	VkPipelineStageFlags stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSemaphore signals[] = { Graphics::renderFinishedSemaphores[Graphics::currentFrame]};
	//for each GfxObject updateUbo[cameraIndex];
    
    std::vector<VkCommandBuffer*> commandBuffers;
    


    //for (auto& gfx : BaseObjectGfx::allGfx){
    //    gfx->updateUBO(imageIndex, main->view, main->projection);
    // }
	

    //should add offset each cameraF
    //cameras with index 1;
	int offset = MAX_FRAMES_IN_FLIGHT;
	//main should always be first camera
    //for (int c = 1; c < Camera::allCameras.size(); c++){//minus main
		//for (auto& gfx : BaseObjectGfx::allGfx) {
							//index ubo,		//index cameras
        //    gfx->updateUBO(c + offset, Camera::allCameras[c]->view, Camera::allCameras[c]->projection);
        //}
    //}



	Image::transitionLayout(main->images[imageIndex], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;	
	submitInfo.waitSemaphoreCount = 1;//sizeof(waits);
	submitInfo.pWaitSemaphores = waits;
	submitInfo.pWaitDstStageMask = stages;
	submitInfo.commandBufferCount = 1; //	may need [main->cmdbuff[imageindex], allOtherCameracmdbuffers]
	submitInfo.pCommandBuffers = &main->commandBuffers[imageIndex];//&commandBuffers[imageIndex];//multiple cmdBuffers???
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signals;



	isValidGfx(vkResetFences(Graphics::logiDevice, 1, &Graphics::fences[Graphics::currentFrame]));
					//graphicsQueue

	isValidGfx(vkQueueSubmit(Graphics::presentQueue, 1, &submitInfo, Graphics::fences[Graphics::currentFrame]));


	

	VkSwapchainKHR swapChains[] = { Graphics::swapChain };
	
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signals;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	Image::transitionLayout(main->images[imageIndex], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	
	isValidGfx(vkQueuePresentKHR(Graphics::presentQueue, &presentInfo));

	isValidGfx(vkQueueWaitIdle(Graphics::presentQueue));



	Graphics::currentFrame ++;
	Graphics::currentFrame  %= MAX_FRAMES_IN_FLIGHT;
}


Camera::~Camera(){
    if (main == this){
        main = nullptr;
    }
    for(int c = 0; c < allCameras.size(); c++){
        auto cam = allCameras[c];
        if(cam == this){
            allCameras.erase(allCameras.begin() + c);
        }
    }


    for(auto buffer : frameBuffers){
        vkDestroyFramebuffer(Graphics::logiDevice, buffer, nullptr);
    }
    for(int i = 0; i < images.size(); i++ ){
        vkDestroyImage(Graphics::logiDevice, images[i], nullptr);
        vkDestroyImageView(Graphics::logiDevice, imageViews[i], nullptr);
    }
    vkDestroyImage(Graphics::logiDevice, depthImage, nullptr);
    vkDestroyImageView(Graphics::logiDevice, depthView, nullptr);
    vkFreeMemory(Graphics::logiDevice, diMemory, nullptr);
    vkDestroyRenderPass(Graphics::logiDevice, renderPass, nullptr);
}

//update()//update to transform component
void Camera::update() {
	static float3 forward = {};
	
	mat4 rotationY = mat4::rotateY(cameraTransform->rotation.y);//leftright

	mat4 translation = mat4::translate({ -cameraTransform->position.x, -cameraTransform->position.y, -cameraTransform->position.z });

																
	//mat4 rotationX = mat4::rotateX(Input::mouse_y);//updown

	//mat4 rotation = rotationX * rotationY;
	//lookAt();
	
	//float3 ahead = { 0.0f, 0.0f, 2.0f };
	//lookAt(ahead);
	
	
	view = translation * rotationY;
	//view = view + cameraTransform->position;//translation;// translation;// * rotationY;

}