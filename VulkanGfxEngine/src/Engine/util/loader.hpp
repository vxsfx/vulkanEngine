#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>

#include <vector>
#include "math.hpp"
#include "buffer.hpp"

#ifndef LOADER
#define LOADER
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
namespace Load {
    inline void Model(const char path[], std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace |
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);
		if (scene == nullptr) { throw std::runtime_error(importer.GetErrorString()); }
		if (!scene->HasMeshes()) { throw std::runtime_error("invalid model:: no meshes"); }

		vertices.clear();
		indices.clear();

		uint32_t indSize = 0;
		uint32_t vertSize = 0;

		/*allocate space
		for (int m = 0; m < scene->mNumMeshes; m++ ) {
			indSize += scene->mMeshes[m]->mNumFaces;
			vertSize += scene->mMeshes[m]->mNumVertices;
		}
		vertices.resize(vertSize);
		indices.resize(indSize);
		*/
        auto& meshes = scene->mMeshes;
		//for (auto& mesh : scene->mMeshes) {
            for(int v = 0; v < meshes[0]->mNumVertices; v++){
                vertices.push_back(
                    {
                        {meshes[0]->mVertices[v].x, meshes[0]->mVertices[v].y, meshes[0]->mVertices[v].z},
                        {meshes[0]->mTextureCoords[0][v].x, 1.0f - meshes[0]->mTextureCoords[0][v].y}
                    }

                );
            }

            for (int f = 0; f < meshes[0]->mNumFaces; f++){
                auto& face = meshes[0]->mFaces[f];
                if(face.mNumIndices == 3){
                    indices.push_back(face.mIndices[0]);
                    indices.push_back(face.mIndices[1]);
                    indices.push_back(face.mIndices[2]);
                }
            }
        //}
	}
	inline void Texture(const char path[], VkBuffer& stagingBuffer, VkDeviceMemory& sbMemory, int sizes[2]) {
		int texWidth;
		int texHeight;
		int texChannels;

		stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		VkDeviceSize imageSize = texWidth * texHeight * 4;//no_pixels * no_rgba

		if (!pixels) {
			throw std::runtime_error("notexture loaded");
		}


		Buffer::createAllocateAndBind(stagingBuffer, sbMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(Graphics::logiDevice, sbMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(Graphics::logiDevice, sbMemory);

		stbi_image_free(pixels);
		sizes[0] = texWidth;
		sizes[1] = texHeight;
	};


    inline std::vector<char> Shader(std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open shader" + path);
		}

		size_t size = (size_t)file.tellg();
		std::vector<char> buffer(size);

		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return buffer;
    };

    inline VkShaderModule ShaderModule(const std::vector<char>& code) {
	    VkShaderModuleCreateInfo info{};
	    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	    info.codeSize = code.size();
	    info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	    VkShaderModule module;
	    if (vkCreateShaderModule(Graphics::logiDevice, &info, nullptr, &module) != VK_SUCCESS) {
		    throw std::runtime_error("failed shader module");
	    }

	    return module;
    }
}

#endif


