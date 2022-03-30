struct Pipeline {
    std::vector<VkShaderModules> modules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderInfos;




    Pipeline& addShader(Pipeline* pipelineData, char* path){}

    Pipeline& addViewPort(){}
    void addRasterizor(){}

    

    void finish(layout, renderPass){
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = {};
        pipelineInfo.stageCount = shaderInfos.size();
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
        isValidGfx((vkCreateGraphicsPipelines(Graphics::logiDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline)));

        Pipeline::destroyShadermodules(temp);
    }

};

struct DescriptorSet{


    Pipeline& addTexture(){}
}

/**
 * @brief 
 * 
 */