#include "BaseObjectGfx.h"
#include <array>
//#include "../util/math.hpp"
#include "../../util/loader.hpp"
#include "../../util/image.hpp"
#include "../../util/buffer.hpp"
#include <vulkan/vulkan.h>

//#include <chrono>


class Model : public BaseObjectGfx {
public:
    Model(const char giveName[], const char modelPath[], const char texPath[] = nullptr);


    std::vector<VkBuffer> uBuffers;
    std::vector<VkDeviceMemory> ubMemory;

    VkImage* textureImage;
    VkDeviceMemory* textureMemory;
    VkImageView* textureView;
    VkSampler* textureSampler;

    mat4 model;

    struct matrices {
        mat4 model;
        mat4 view;
        mat4 proj;
    };

    void createDescriptorSets(int imageCount, std::vector<VkDescriptorImageInfo>& imageInfos) override;
    void createDescriptorSetLayout() override;

    void createVertexBuffer();
    void createIndexBuffer();
    void createBuffers(int count);

    void createTexture(const char texPath[]);


    void createPipeline(VkRenderPass& renderPass, VkExtent2D extent) override;

    void createUBOs(int count) override;
    void updateUBO(int index, mat4 view, mat4 proj) override;
    //createAllPipelines()//use Camera::allCameras

    void update() override;

    ~Model();

};