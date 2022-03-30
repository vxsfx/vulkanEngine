#include "Primitives.h"

/*
change too highly polymorphic design

i.e

class {
	void (*method1)();
	void (*method2)();
	//where methods can be assigned
	
}

*/



Primitive::Primitive(std::string giveName, int id) : BaseObjectGfx(giveName)
{
	PrimID = id;
}
void Primitive::addTransform(Transform* tf) {
	transform = tf;
}

//EXPLICITLY PASS IN vertices, for creating
Rectangle::Rectangle(std::string givename, std::vector<Vertex>* verts ) : Primitive(givename, 1) {
	vertices = *verts;

	indices = {
		0, 1, 2,
		2, 3, 0
	};

}


//SHOULD MOVE TO BaseObjectGfx;
void Rectangle::createVertexBuffer()
{
	VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory sbMemory;
	isValid(Buffer::create(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT));
	isValid(Buffer::allocateAndBind(stagingBuffer, sbMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

	void* data;
	isValidGfx(vkMapMemory(Graphics::logiDevice, sbMemory, 0, size, 0, &data));
	memcpy(data, vertices.data(), (size_t)size);
	vkUnmapMemory(Graphics::logiDevice, sbMemory);

	isValid(Buffer::create(vertexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
	isValid(Buffer::allocateAndBind(vertexBuffer, vbMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	Buffer::copy(stagingBuffer, vertexBuffer, size);

	vkDestroyBuffer(Graphics::logiDevice, stagingBuffer, nullptr);
	vkFreeMemory(Graphics::logiDevice, sbMemory, nullptr);

	VkResult res = vkMapMemory(Graphics::logiDevice, vbMemory, 0, size, 0, &data);
	//isValidGfx(res);//was not looged previously


	memcpy(data, vertices.data(), (size_t)size);

	vkUnmapMemory(Graphics::logiDevice, vbMemory);
}

void Rectangle::createIndexBuffer()
{
	VkDeviceSize size = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory sbMemory;

	isValid(Buffer::create(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT));
	isValid(Buffer::allocateAndBind(stagingBuffer, sbMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

	void* data;
	isValidGfx(vkMapMemory(Graphics::logiDevice, sbMemory, 0, size, 0, &data));
	memcpy(data, indices.data(), (size_t)size);
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

void Rectangle::createBuffers(int count)
{
	createVertexBuffer();
	createIndexBuffer();
	createUBOs(count);
}

void Rectangle::createUBOs(int count)
{
	VkDeviceSize size = sizeof(matrices);

	uBuffers.resize(count);
	ubMemory.resize(count);

	for (int i = 0; i < count; i++) {
		if (!Buffer::createAllocateAndBind(uBuffers[i], ubMemory[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			throw std::runtime_error("failed to create a unifomr buffer");
		}
	}
}

//image count, total count of all images of every camera 
void Rectangle::createDescriptorSets(int imageCount, std::vector<VkDescriptorImageInfo>& imageInfos)
{
	createDescriptorSetLayout();
	std::vector<VkDescriptorSetLayout> layouts(imageCount, descSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = Graphics::descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(imageCount);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(imageCount);
	VkResult res = vkAllocateDescriptorSets(Graphics::logiDevice, &allocInfo, descriptorSets.data());
	isValidGfx(res);
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

void Rectangle::update()
{
	
	mat4 rotateY = mat4::rotateY(transform->rotation.y);
	mat4 rotateX = mat4::rotateX(transform->rotation.x);
	mat4 rotateZ = mat4::rotateZ(transform->rotation.z);

	mat4 model = rotateX * rotateY * rotateZ;
	mat4 world = mat4::translate(transform->position);
	
	modelWorld = model;//{
		//{1.0f, 0.0f, 0.0f, 0.0f},
		//{0.0f, 1.0f, 0.0f, 0.0f},
		//{0.0f, 0.0f, 1.0f, 0.0f},
		//{0.0f, 0.0f, 0.0f, 1.0f}
	//};
	

	for (int c = 0; c < Camera::allCameras.size(); c++) {
		auto camera = Camera::allCameras[c];
		uint32_t uboIndex = c * MAX_FRAMES_IN_FLIGHT + Graphics::currentFrame;//each camera has MAX frameBufferImage size, +currentFrame for currentframe of camera
		updateUBO(uboIndex, camera->view, camera->projection);
	}


}

void Rectangle::createDescriptorSetLayout() {
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

void Rectangle::createPipeline(VkRenderPass& renderPass, VkExtent2D extent) {
	PipelineBuilder* builder = new PipelineBuilder();

	builder->addShader("./res/shaders/imageVertex.spv", VK_SHADER_STAGE_VERTEX_BIT);
	builder->addShader("./res/shaders/imageFragment.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	//temp not member
	std::vector<VkDescriptorSetLayout> layouts = { descSetLayout };

	builder
		->addVertexInput()
		.addViewport(extent)
		.addIA()
		.addDynamicStates(dynamicStates)
		.addRasterizor()
		.addBlend()
		.addDepthStencil()
		.addSampler()
		.createLayout(layouts, pipelineLayout);



	isValidGfx(builder->createPipeline(pipeline, pipelineLayout, renderPass));
	delete builder;
}

void Rectangle::updateUBO(int index, mat4 view, mat4 proj) {
	matrices mats{};
	mats.view = view;
	mats.proj = proj;
	mats.modelWorld = modelWorld;

	void* data;
	vkMapMemory(Graphics::logiDevice, ubMemory[index], 0, sizeof(mats), 0, &data);
	memcpy(data, &mats, sizeof(mats));
	vkUnmapMemory(Graphics::logiDevice, ubMemory[index]);
}

void Rectangle::createTexure(const char texPath[]) {
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

Primitive::~Primitive() {
	for (int b = 0; b < uBuffers.size(); b++) {
		vkDestroyBuffer(Graphics::logiDevice, uBuffers[b], nullptr);
		vkFreeMemory(Graphics::logiDevice, ubMemory[b], nullptr);
	}
}