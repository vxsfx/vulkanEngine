#include "Model.h"

Model::Model(const char giveName[], const char modelPath[], const char texPath[]) : BaseObjectGfx(giveName){
    //loadVertices()//loadModel
    Load::Model(modelPath, vertices, indices);
    
    if(texPath != nullptr) {
        //createTexture
		createTexture(texPath);
    }
}

void Model::createTexture(const char texPath[]) {
	VkBuffer stagingBuffer;
	VkDeviceMemory sbMemory;

	//add texture
	int sizes[2];
	Load::Texture(texPath, stagingBuffer, sbMemory, sizes);
	//createImageTexture
	textureImage = new VkImage();
	textureMemory = new VkDeviceMemory();
	Image::create(textureImage, sizes[0], sizes[1], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

	Image::bindMemory(*textureImage, *textureMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	Image::transitionLayout(*textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Buffer::copyBufferToImage(stagingBuffer, *textureImage, static_cast<uint32_t>(sizes[0]), static_cast<uint32_t>(sizes[1]));
	Image::transitionLayout(*textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vkDestroyBuffer(Graphics::logiDevice, stagingBuffer, nullptr);
	vkFreeMemory(Graphics::logiDevice, sbMemory, nullptr);

	textureView = new VkImageView();
	//createView 
	Image::createView(textureView, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

	textureSampler = new VkSampler();
	Image::createSampler(textureSampler);
}

void Model::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = nullptr;


	VkDescriptorSetLayoutBinding samplerBinding{};
	samplerBinding.binding = 1;
	samplerBinding.descriptorCount = 1;
	samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerBinding.pImmutableSamplers = nullptr;
	samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { layoutBinding, samplerBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(Graphics::logiDevice, &layoutInfo, nullptr, &descSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("faioloed layout binding");
	}

}

//if count 2, views 1, use create vector of 2 ref to same image
void Model::createDescriptorSets(int imageCount, std::vector<VkDescriptorImageInfo>& imageInfos){
	createDescriptorSetLayout();
    std::vector<VkDescriptorSetLayout> layouts(imageCount, descSetLayout);
	
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = Graphics::descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(imageCount);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(imageCount);
	isValidGfx(vkAllocateDescriptorSets(Graphics::logiDevice, &allocInfo, descriptorSets.data()));
	for (int i = 0; i < imageCount; i++) {
		std::array<VkWriteDescriptorSet, 2> descWrites{};
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(matrices);

		// VkDescriptorImageInfo imageInfo{};
		// imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// imageInfo.imageView = imageViews[i];//textureView
		// imageInfo.sampler = texSampler;


		descWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrites[0].dstSet = descriptorSets[i];
		descWrites[0].dstBinding = 0;
		descWrites[0].dstArrayElement = 0;
		descWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descWrites[0].descriptorCount = 1;
		descWrites[0].pBufferInfo = &bufferInfo;

		descWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrites[1].dstSet = descriptorSets[i];
		descWrites[1].dstBinding = 1;
		descWrites[1].dstArrayElement = 0;
		descWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descWrites[1].descriptorCount = 1;
		descWrites[1].pImageInfo = &imageInfos[i];

		vkUpdateDescriptorSets(Graphics::logiDevice, static_cast<uint32_t>(descWrites.size()),
			descWrites.data(), 0, nullptr);

	}
}


#pragma region Buffers
//can each be abstracted to single buffers functi0n

void Model::createVertexBuffer(){
    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
	
	VkBuffer stagingBuffer;
	VkDeviceMemory sbMemory;
    isValid(Buffer::create(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT));
    isValid(Buffer::allocateAndBind(stagingBuffer, sbMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

    void* data;
	isValidGfx(vkMapMemory(Graphics::logiDevice, sbMemory, 0, size, 0, &data));
	memcpy(data, vertices.data(), (size_t) size);
	vkUnmapMemory(Graphics::logiDevice, sbMemory);

    isValid(Buffer::create(vertexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
    isValid(Buffer::allocateAndBind(vertexBuffer, vbMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    Buffer::copy(stagingBuffer, vertexBuffer, size);



	vkDestroyBuffer(Graphics::logiDevice, stagingBuffer, nullptr);
	vkFreeMemory(Graphics::logiDevice, sbMemory, nullptr);

	VkResult res = vkMapMemory(Graphics::logiDevice, vbMemory, 0, size, 0, &data);
	
	
	//isValidGfx(res);//was not looged previously


	memcpy(data, vertices.data(), (size_t)size);


	bool a = false;

	vkUnmapMemory(Graphics::logiDevice, vbMemory);
}

void Model::createIndexBuffer(){
    VkDeviceSize size = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory sbMemory;

    isValid(Buffer::create(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT));
    isValid(Buffer::allocateAndBind(stagingBuffer, sbMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

    void* data;
	isValidGfx(vkMapMemory(Graphics::logiDevice, sbMemory, 0, size, 0, &data));
	memcpy(data, indices.data(), (size_t) size);
	vkUnmapMemory(Graphics::logiDevice, sbMemory);


    isValid(Buffer::create(indexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
    isValid(Buffer::allocateAndBind(indexBuffer, ibMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    Buffer::copy(stagingBuffer, indexBuffer, size);
	vkDestroyBuffer(Graphics::logiDevice, stagingBuffer, nullptr);
	vkFreeMemory(Graphics::logiDevice, sbMemory, nullptr);

	vkMapMemory(Graphics::logiDevice, ibMemory, 0, size, 0, &data);//isValidGfx
	memcpy(data, indices.data(), (size_t)size);
	vkUnmapMemory(Graphics::logiDevice, ibMemory);

}

//need addUbo too
void Model::createUBOs(int count){
    VkDeviceSize size = sizeof(matrices);

	uBuffers.resize(count);
	ubMemory.resize(count);

	for (int i = 0; i < count; i++) {
		if (!Buffer::createAllocateAndBind(uBuffers[i], ubMemory[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
        {
			throw std::runtime_error("failed to create a unifomr buffer");
		}
	}
}



void Model::createBuffers(int count){
    //go through if indices etc.

    createVertexBuffer();
    createIndexBuffer();
    createUBOs(count);
}
#pragma endregion


#pragma region Pipeline


//should pass camera
void Model::createPipeline(VkRenderPass& renderPass, VkExtent2D extent){
	/*
    //load model shaders
	std::vector<VkShaderModule> temp;
	VkPipelineShaderStageCreateInfo shaderInfos[] = {
		//path from proj folder
        Pipeline::addShader("./res/shaders/imageVertex.spv", VK_SHADER_STAGE_VERTEX_BIT, temp),
        Pipeline::addShader("./res/shaders/imageFragment.spv", VK_SHADER_STAGE_FRAGMENT_BIT, temp)
    };

    auto bindDesc = Vertex::getBindingDescription();
	auto attDesc = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertInInfo{};
	vertInInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInInfo.vertexBindingDescriptionCount = 1;
	vertInInfo.pVertexBindingDescriptions = &bindDesc; //opt
	vertInInfo.vertexAttributeDescriptionCount = static_cast<uint32_t> (attDesc.size());
	vertInInfo.pVertexAttributeDescriptions = attDesc.data(); //opt

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable= VK_FALSE;

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo stateInfo{};
	stateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	stateInfo.dynamicStateCount = 2;
	stateInfo.pDynamicStates = dynamicStates;
	stateInfo.pNext = {};

    
	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	
	//OPT
	layoutInfo.pNext = {};
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &descSetLayout;
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = nullptr;
	
	isValidGfx(vkCreatePipelineLayout(Graphics::logiDevice, &layoutInfo, nullptr, &pipelineLayout));

	//BUILDER PATTERN
    //should if through flags else nullptr for infos
	auto vp = Pipeline::addViewport(extent);
	auto rast = Pipeline::addRasterizor();
	auto samp = Pipeline::addSampler();
	auto stenc = Pipeline::addDepthStencil();
    auto blend = Pipeline::addBlend();
	
	//createPipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = {};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderInfos;
	pipelineInfo.pVertexInputState = &vertInInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &vp;///////////////////////////////////
	pipelineInfo.pRasterizationState = &rast;
	pipelineInfo.pMultisampleState = &samp;
	pipelineInfo.pDepthStencilState = &stenc;//3D
	pipelineInfo.pColorBlendState = &blend;
	pipelineInfo.pDynamicState = nullptr;//OPT
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	//OPT
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	//multi pipelines maybe for multi object types???>??
	isValidGfx( (vkCreateGraphicsPipelines(Graphics::logiDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline)));

    Pipeline::destroyShadermodules(temp);
	*/
}

#pragma endregion

void Model::update() {
	//static auto start = std::chrono::high_resolution_clock::now();

	//auto cur = std::chrono::high_resolution_clock::now();
	//float t = std::chrono::duration<float, std::chrono::seconds::period>(cur - start).count();
	float t = 1.0f;
	//model = {
	//	{1.0f * cos(t), 0.0f, 1.0f * sin(t),  0.0f},
	//	{0.0f, 1.0f, 0.0f, 0.0f},
	//	{1.0f * -sin(t), 0.0f, 1.0f * cos(t),  0.0f},
	//	{0.0f,	0.0f, 0.0f,  1.0f}
	//};
	model = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

}

//move to baseObjgfx
void Model::updateUBO(int index, mat4 view, mat4 proj){
    matrices mats{};
    mats.view = view;
    mats.proj = proj;
    mats.model = model;

    void* data;
	vkMapMemory(Graphics::logiDevice, ubMemory[index], 0, sizeof(mats), 0, &data);
	memcpy(data, &mats, sizeof(mats));
	vkUnmapMemory(Graphics::logiDevice, ubMemory[index]);
}


Model::~Model(){
    //vkDestroyImage(lDevice, texture, nullptr);
	//vkFreeMemory(lDevice, texMemory, nullptr);

    for(int b = 0; b < uBuffers.size(); b++){
        vkDestroyBuffer(Graphics::logiDevice, uBuffers[b], nullptr);
        vkFreeMemory(Graphics::logiDevice, ubMemory[b], nullptr);
    }


}