#include "Component.h"
#include <vulkan/vulkan.h>
#include "../../Graphics.h"
#include "../../util/Pipeline.hpp"


#ifndef ObjGfx
#define ObjGfx
class BaseObjectGfx : public Component {
public:
    const COMPONENT_TYPE type = COMPONENT_TYPE::GRAPHICS;
    
    inline static std::vector<BaseObjectGfx*> allGfx;//vector of ref not allowed???

    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descSetLayout;

    VkBuffer vertexBuffer;
    VkDeviceMemory vbMemory;
    VkBuffer indexBuffer;
	VkDeviceMemory ibMemory;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    virtual void createDescriptorSetLayout() = 0;
    virtual void createDescriptorSets(int imageCount, std::vector<VkDescriptorImageInfo>& imageInfos) = 0;
    //no ubos here //can be inherited by ui/hud elelemtnes

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    BaseObjectGfx(std::string giveName);
    ~BaseObjectGfx();


    virtual void createPipeline(VkRenderPass& renderPass, VkExtent2D extent) = 0;
    virtual void updateUBO(int index, mat4 view, mat4 proj) = 0;
    virtual void createUBOs(int cout) = 0;
    //virtual delet ubos
};


#endif