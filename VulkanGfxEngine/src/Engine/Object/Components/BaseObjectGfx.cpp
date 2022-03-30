#include "BaseObjectGfx.h"



BaseObjectGfx::BaseObjectGfx(std::string giveName) : Component(giveName) {
    allGfx.push_back(this);
};


BaseObjectGfx::~BaseObjectGfx() {
for (int i = 0; i < allGfx.size(); i++) {
    auto obj = allGfx[i];
    if (obj == this) {
        allGfx.erase(allGfx.begin() + i);
    }
}
vkDestroyBuffer(Graphics::logiDevice, vertexBuffer, nullptr);
vkDestroyBuffer(Graphics::logiDevice, indexBuffer, nullptr);
vkFreeMemory(Graphics::logiDevice, ibMemory, nullptr);
vkFreeMemory(Graphics::logiDevice, vbMemory, nullptr);

vkDestroyPipeline(Graphics::logiDevice, pipeline, nullptr);
vkDestroyPipelineLayout(Graphics::logiDevice, pipelineLayout, nullptr);
vkDestroyDescriptorSetLayout(Graphics::logiDevice, descSetLayout, nullptr);
    }