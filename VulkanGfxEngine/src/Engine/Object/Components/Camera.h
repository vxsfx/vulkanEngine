//#include "Component.h"
#include <vulkan/vulkan.h>
#include "../../Graphics.h"
#include "BaseObjectGfx.h"
#include "Transform.h"
#include "../../util/math.hpp"
#include "../../util/image.hpp"

class Camera : public Component {
public:
    COMPONENT_TYPE type = COMPONENT_TYPE::CAMERA;
    inline static Camera* main;
    inline static std::vector<Camera*> allCameras;
    inline static std::vector<VkRenderPass*> allRenderPasses;

    //camera tfs
    Transform* cameraTransform;
    mat4 projection;
    mat4 view;

    void lookAt(float3 towards);


    //uint32_t currentFrame;


    //create buffers for camera images
    std::vector<VkFramebuffer> frameBuffers;
    void createFrameBuffers();

    VkRenderPass renderPass;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

    VkImage depthImage;
    VkDeviceMemory diMemory;
    VkImageView depthView;


    VkFormat imageFormat;
    VkFormat depthFormat;

    VkExtent2D extent;



    std::vector<VkCommandBuffer> commandBuffers;
    
    
    
    Camera(std::string givename, int width, int height, int count);    
    void createImageViews(VkFormat format);
    void createDepthResource();

    void createRenderPass();

    void setProjection(float ratio, float fov, float near, float far);

    std::vector<VkDescriptorImageInfo> getDescriptorImageInfos();

    void createCommandBuffers();

    static void draw();

    void update() override;

    ~Camera();
};