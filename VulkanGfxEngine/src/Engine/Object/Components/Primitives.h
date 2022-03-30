#pragma once
#include "BaseObjectGfx.h"
#include "../../util/image.hpp"
#include "../../util/buffer.hpp"
#include "Transform.h"
#include "Camera.h"

//#include <chrono>

class Primitive : public BaseObjectGfx {
public:
	int PrimID;
	Primitive(std::string giveName, int id);
	void addTransform(Transform* tf);

	Transform* transform;
	
	std::vector<VkBuffer> uBuffers;
	std::vector<VkDeviceMemory> ubMemory;

	VkImage* textureImage;
	VkDeviceMemory* textureMemory;
	VkImageView* textureView;
	VkSampler* textureSampler;

	mat4 modelWorld;

	struct matrices {
		mat4 modelWorld;
		mat4 view;
		mat4 proj;
	};

	~Primitive();
};

class Rectangle : public Primitive {
public:
	Rectangle(std::string giveName, std::vector<Vertex>* verts);

	void createVertexBuffer();
	void createIndexBuffer();
	void createBuffers(int count);

	void createDescriptorSetLayout() override;
	void createDescriptorSets(int imageCount, std::vector<VkDescriptorImageInfo>& imageInfos) override;

	void createPipeline(VkRenderPass& renderPass, VkExtent2D extent) override;

	void createUBOs(int count) override;
	void updateUBO(int index, mat4 view, mat4 proj) override;
	//createAllPipelines()//use Camera::allCameras

	void update() override;

	//use before camera Textures
	void createTexure(const char texPath[]);
};

