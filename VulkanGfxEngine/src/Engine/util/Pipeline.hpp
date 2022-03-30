#include <vulkan/vulkan.h>
#include "loader.hpp"

#ifndef PIPELINE
#define PIPELINE

namespace Pipeline {
    //if add not used, default will be nullptr


    enum PipelineFlags {//each struct property to be used here aswell i.e RASTER_CLAMP_TRUE
        Shader =   1<<0,
        Viewport = 1<<1,//
        Raster =   1<<2,//
        Sampler =  1<<3,//
        Blend =    1<<4,//
    };   
}

struct PipelineBuilder {
    
    std::vector<VkShaderModule> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;




    VkPipelineDepthStencilStateCreateInfo* depthStencil = nullptr;

    //for textures 
    VkPipelineMultisampleStateCreateInfo* sampInfo = nullptr;

    VkPipelineColorBlendStateCreateInfo* blendInfo = nullptr;

    VkPipelineViewportStateCreateInfo* vpInfo = nullptr;


    VkPipelineRasterizationStateCreateInfo* rastInfo = nullptr;

    VkPipelineInputAssemblyStateCreateInfo* inputAssembly = nullptr;

    VkPipelineVertexInputStateCreateInfo* vertInInfo = nullptr;
    VkVertexInputBindingDescription* bindDesc = nullptr;
    std::vector<VkVertexInputAttributeDescription> attDescs;


    VkPipelineDynamicStateCreateInfo* stateInfo = nullptr;




    //inline PipelineBuilder() {}

    /// <summary>
    /// adds the shader from specified path, and provide type(i.e., VK_SHADER_STAGE_VERTEX_BIT)
    /// </summary>
    inline PipelineBuilder& addShader(std::string path, VkShaderStageFlagBits type) {
        auto shader = Load::Shader(path);
        VkShaderModule module = Load::ShaderModule(shader);
        shaderModules.push_back(module);

        VkPipelineShaderStageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.stage = type;
        info.module = module;
        info.pName = "main";

        shaderInfos.push_back(info);

        return *this;
    }

    /// <summary>
    /// adds a depths stencil to pipeline
    /// </summary>
    /// <returns>this</returns>
    inline PipelineBuilder& addDepthStencil() {
        depthStencil = new VkPipelineDepthStencilStateCreateInfo();
        depthStencil->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil->depthTestEnable = VK_TRUE;
        depthStencil->depthWriteEnable = VK_TRUE;
        (*depthStencil).depthCompareOp = VK_COMPARE_OP_LESS;///////* suppresses green squiggle??
        depthStencil->depthBoundsTestEnable = VK_FALSE;
        depthStencil->minDepthBounds = 0.0f;
        depthStencil->maxDepthBounds = 1.0f;
        depthStencil->stencilTestEnable = VK_TRUE;
        depthStencil->front = {};
        depthStencil->back = {};
        
        return *this;
    }

    /// <summary>
    /// adds sampler to pipeline for filtering and stuff
    /// </summary>
    /// <returns>this</returns>
    inline PipelineBuilder& addSampler() {
        sampInfo = new VkPipelineMultisampleStateCreateInfo();
        sampInfo->pNext = {};
        sampInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        sampInfo->sampleShadingEnable = VK_FALSE;
        (*sampInfo).rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        //OPT
        sampInfo->minSampleShading = 1.0f;
        sampInfo->pSampleMask = nullptr;
        sampInfo->alphaToCoverageEnable = VK_FALSE;
        sampInfo->alphaToOneEnable = VK_FALSE;

        return *this;
    };

    /// <summary>
    /// adds blend attachment
    /// </summary>
    /// <returns>this</returns>
    inline PipelineBuilder& addBlend() {
        VkPipelineColorBlendAttachmentState* blendAttachment = new VkPipelineColorBlendAttachmentState();
        blendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachment->blendEnable = VK_FALSE;
        //OPT
        (*blendAttachment).srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        (*blendAttachment).dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        (*blendAttachment).colorBlendOp = VK_BLEND_OP_ADD;
        (*blendAttachment).srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        (*blendAttachment).dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        (*blendAttachment).alphaBlendOp = VK_BLEND_OP_ADD;

        blendInfo = new VkPipelineColorBlendStateCreateInfo();
        blendInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendInfo->pNext = {};
        blendInfo->logicOpEnable = VK_FALSE;
        blendInfo->attachmentCount = 1;
        blendInfo->pAttachments = blendAttachment;
        //OPT
        (*blendInfo).logicOp = VK_LOGIC_OP_COPY;
        blendInfo->blendConstants[0] = 0.0f;
        blendInfo->blendConstants[1] = 0.0f;
        blendInfo->blendConstants[2] = 0.0f;
        blendInfo->blendConstants[3] = 0.0f;
        return *this;
    }

    inline PipelineBuilder& addViewport(VkExtent2D& extent) {
        //pointer for scope
        VkViewport* vp = new VkViewport();
        vp->x = 0.f;
        vp->y = 0.f;
        vp->width = (float)extent.width;
        vp->height = (float)extent.height;
        vp->minDepth = 0.f;
        vp->maxDepth = 1.f;

        VkRect2D* scissor = new VkRect2D();
        scissor->offset = { 0,0 };
        scissor->extent = extent;

        vpInfo = new VkPipelineViewportStateCreateInfo();
        vpInfo->pNext = {};
        vpInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vpInfo->viewportCount = 1;
        vpInfo->pViewports = vp;
        vpInfo->scissorCount = 1;
        vpInfo->pScissors = scissor;
        return *this;
    }

    inline PipelineBuilder& addRasterizor() {
        rastInfo = new VkPipelineRasterizationStateCreateInfo();
        rastInfo->flags = 0;
        rastInfo->pNext = {};
        rastInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rastInfo->depthClampEnable = VK_FALSE;
        rastInfo->rasterizerDiscardEnable = VK_FALSE;
        (*rastInfo).polygonMode = VK_POLYGON_MODE_FILL;
        rastInfo->lineWidth = 1.f;
        rastInfo->cullMode = VK_CULL_MODE_BACK_BIT;
        (*rastInfo).frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rastInfo->depthBiasEnable = VK_FALSE;
        //OPT
        rastInfo->depthBiasConstantFactor = 0.f;
        rastInfo->depthBiasClamp = 0.0f;
        rastInfo->depthBiasSlopeFactor = 0.0f;
        return *this;
    }

    /// <summary>
    /// add inputAssembly
    /// </summary>
    inline PipelineBuilder& addIA() {
        inputAssembly = new VkPipelineInputAssemblyStateCreateInfo();
        inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        (*inputAssembly).topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly->primitiveRestartEnable = VK_FALSE;
        return *this;
    }

    inline PipelineBuilder& addVertexInput() {
        bindDesc = Vertex::getBindingDescription();
        attDescs = Vertex::getAttributeDescriptions();

        vertInInfo = new VkPipelineVertexInputStateCreateInfo();
        vertInInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertInInfo->vertexBindingDescriptionCount = 1;
        vertInInfo->pVertexBindingDescriptions = bindDesc; //opt   ///////<- err
        vertInInfo->vertexAttributeDescriptionCount = static_cast<uint32_t> (attDescs.size());
        vertInInfo->pVertexAttributeDescriptions = attDescs.data(); //opt
        return *this;
    }

    inline PipelineBuilder& addDynamicStates(std::vector<VkDynamicState>& dynamicStates) {
        stateInfo = new VkPipelineDynamicStateCreateInfo();
        stateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        stateInfo->dynamicStateCount = dynamicStates.size();
        stateInfo->pDynamicStates = dynamicStates.data();
        stateInfo->pNext = {};
        return *this;
    }

    inline PipelineBuilder& createLayout(std::vector<VkDescriptorSetLayout>& descSetLayouts, VkPipelineLayout& pipelineLayout) {

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        //OPT
        layoutInfo.pNext = {};
        layoutInfo.setLayoutCount = descSetLayouts.size();
        layoutInfo.pSetLayouts = descSetLayouts.data();
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;

        isValidGfx(vkCreatePipelineLayout(Graphics::logiDevice, &layoutInfo, nullptr, &pipelineLayout));
        return *this;
    }

    inline VkResult createPipeline(VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, VkRenderPass& renderPass) {


        //createPipeline
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = {};
        pipelineInfo.stageCount = shaderInfos.size();
        pipelineInfo.pStages = shaderInfos.data();
        pipelineInfo.pVertexInputState = vertInInfo;
        pipelineInfo.pInputAssemblyState = inputAssembly;
        pipelineInfo.pViewportState = vpInfo;
        pipelineInfo.pRasterizationState = rastInfo;
        pipelineInfo.pMultisampleState = sampInfo;
        pipelineInfo.pDepthStencilState = depthStencil;//3D
        pipelineInfo.pColorBlendState = blendInfo;
        pipelineInfo.pDynamicState = nullptr;//OPT
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;//need methods to add subpasses
        //OPT
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        //multi pipelines maybe for multi object types???>??
        return(vkCreateGraphicsPipelines(Graphics::logiDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline));
    }

    inline ~PipelineBuilder() {
        for (auto module : shaderModules) {
            vkDestroyShaderModule(Graphics::logiDevice, module, nullptr);
        }
        if (shaderModules.size() != 0) {
            shaderModules.clear();
        }
        shaderInfos.clear();

        delete rastInfo;
        delete depthStencil;
        delete sampInfo;
        delete vpInfo;
        delete blendInfo;
        delete bindDesc;

        attDescs.clear();
    }
};

#endif