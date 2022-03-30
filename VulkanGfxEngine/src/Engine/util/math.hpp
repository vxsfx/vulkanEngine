#include <vulkan/vulkan.h>
#include <array>

#ifndef MATHNM
#define MATHNM

struct float2 {
    float x;
    float y;
	
	inline float2 Normalize() {
		float dist = sqrt(x * x + y * y);
		float2 n = { x / dist, y / dist};
		return n;
	}

	inline float dot(float2 other) {
		return (x * other.x + y * other.y);
	}
	

	inline float2 operator+ (float2 other) {
		return { other.x + x, other.y + y};
	}

	inline float2& operator&= (float2 other) {
		return *this;
	}
};

struct float3 : float2{
	//float x;
	//float y;
	float z;

	inline float3 Normalize() {
		float dist = sqrt(x * x + y * y + z * z);
		float3 n = { x / dist, y / dist, z / dist };
		return n;
	}

	inline float dot(float3 other) {
		return (x * other.x + y * other.y + z * other.z);
	}

	inline float3 cross(float3 other) {
		float nx = y * other.z - z * other.y;
		float ny = z * other.x - x * other.z;
		float nz = x * other.y - y * other.x;
		return { nx, ny, nz };
	}

	inline float3 operator+ (float3 other) {
		return { other.x + x, other.y + y, other.z + z };
	}

	/*
	//do same for *= as ptr
	inline float2 operator&= (float2 other) {
		//return { &x, &y };
		float& xn = x;
		float& yn = y;
		return { xn, yn };//return ref to internal floats (actual grab slice)
	}
	*/ //not needed as inherited float2 get of float2 struct
	inline float3& operator&= (float3 other) {
		return *this;
	}
};
//use inheritence to grab other stuff
struct float4 : float3{
	//if normailizing not w, use float4[0:3] normailize
	//float x;
	//float y;
	//float z;
	float w;

	inline float4 Normalize() {
		float dist = sqrt(x * x + y * y + z * z + w * w);
		float4 n = { x / dist, y / dist, z / dist, w / dist };
		return n;
	}

	inline float dot(float4 other) {
		return (x * other.x + y * other.y + z * other.z + w * other.w);
	}


	//get float3 () {return &x, &y, &z}
	inline float4 operator&= (float4 other) {
		return *this;
	};

};

struct mat4 {
	float4 x;
	float4 y;
	float4 z;
	float4 w;

	static inline mat4 lookAt(float3 towards, float3 pos, float3 up) {
		float3 forward = { towards.x - pos.x, towards.y - pos.y, towards.z - pos.z };
		forward = forward.Normalize();

		float3 right = forward.cross(up);
		right = right.Normalize();

		float3 ups = right.cross(forward);
		//ups = ups.Normalize();

		mat4 matrix = {
			{right.x, ups.x, -forward.x, 0.0f},
			{right.y, ups.y, -forward.y, 0.0f},
			{right.z, ups.z, -forward.z, 0.0f},
			{-right.dot(pos), -ups.dot(pos), forward.dot(pos), 1.0f}
		};

		return matrix;
	}

	inline static mat4 rotateX(float angle) {
		return {
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f * cosf(angle), 1.0f * sinf(angle), 0.0f},
			{0.0f, 1.0f * -sinf(angle), 1.0f * cosf(angle), 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

	}
	inline static mat4 rotateY(float angle) {
		return {
			{cosf(angle), 0.0f, sinf(angle), 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{-sinf(angle), 0.0f, cosf(angle), 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
	}
	inline static mat4 rotateZ(float angle) {
		return {
			{1.0f * cosf(angle), 1.0f * sinf(angle), 0.0f, 0.0f},
			{1.0f * -sinf(angle), 1.0f * cosf(angle), 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
	}

	inline static mat4 translate(float3 dir) {
		return {
			{-1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, -1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, -1.0f, 0.0f},
			{dir.x, dir.y, dir.z, 1.0f}//at bottom not side??, maybe glsl mult stupid???????
		};
	}


	inline float3 toAngle() {
		float xang = atan2(z.y, z.z);
		float yang = atan2(-z.x, sqrtf(z.y * z.y + z.z * z.z));
		float zang = atan2(y.x, x.x);
		return { xang, yang, zang };
	}

	
	inline float3 operator* (float3 other) {
		float xn = x.x * other.x + x.y * other.y + x.z * other.z;
		float yn = y.x * other.x + y.y * other.y + y.z * other.z;
		float zn = z.x * other.x + z.y * other.y + z.z * other.z;
		return { xn, yn, zn };
	}

	inline mat4 operator* (mat4 other) {
		mat4 matrix = {};
		matrix.x.x = { x.x * other.x.x + x.y * other.y.x + x.z * other.z.x + x.w * other.w.x };
		matrix.y.x = { y.x * other.x.x + y.y * other.y.x + y.z * other.z.x + y.w * other.w.x };
		matrix.z.x = { z.x * other.x.x + z.y * other.y.x + z.z * other.z.x + z.w * other.w.x };
		matrix.w.x = { w.x * other.x.x + w.y * other.y.x + w.z * other.z.x + w.w * other.w.x };

		matrix.x.y = { x.x * other.x.y + x.y * other.y.y + x.z * other.z.y + x.w * other.w.y };
		matrix.y.y = { y.x * other.x.y + y.y * other.y.y + y.z * other.z.y + y.w * other.w.y };
		matrix.z.y = { z.x * other.x.y + z.y * other.y.y + z.z * other.z.y + z.w * other.w.y };
		matrix.w.y = { w.x * other.x.y + w.y * other.y.y + w.z * other.z.y + w.w * other.w.y };

		matrix.x.z = { x.x * other.x.z + x.y * other.y.z + x.z * other.z.z + x.w * other.w.z };
		matrix.y.z = { y.x * other.x.z + y.y * other.y.z + y.z * other.z.z + y.w * other.w.z };
		matrix.z.z = { z.x * other.x.z + z.y * other.y.z + z.z * other.z.z + z.w * other.w.z };
		matrix.w.z = { w.x * other.x.z + w.y * other.y.z + w.z * other.z.z + w.w * other.w.z };

		matrix.x.w = { x.x * other.x.w + x.y * other.y.w + x.z * other.z.w + x.w * other.w.w };
		matrix.y.w = { y.x * other.x.w + y.y * other.y.w + y.z * other.z.w + y.w * other.w.w };
		matrix.z.w = { z.x * other.x.w + z.y * other.y.w + z.z * other.z.w + z.w * other.w.w };
		matrix.w.w = { w.x * other.x.w + w.y * other.y.w + w.z * other.z.w + w.w * other.w.w };
		return matrix;
	}
};

struct Vertex {
	float3 pos;//pos
	float2 tex;//tex

	inline static VkVertexInputBindingDescription* getBindingDescription() {
		VkVertexInputBindingDescription* bindDesc = new VkVertexInputBindingDescription();
		bindDesc->binding = 0;
		bindDesc->stride = sizeof(Vertex);
		bindDesc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//next entry by vertex
		return bindDesc;
	};
	//std::array<VkVertexInputAttributeDescription, 2>
	inline static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
		//std::array<VkVertexInputAttributeDescription, 2> attDesc{};
		std::vector<VkVertexInputAttributeDescription> attDesc(2);
		//pos
		attDesc[0].binding = 0;
		attDesc[0].location = 0;
		attDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attDesc[0].offset = offsetof(Vertex, pos);

		//tex
		attDesc[1].binding = 0;
		attDesc[1].location = 1;
		attDesc[1].format = VK_FORMAT_R32G32_SFLOAT;
		attDesc[1].offset = offsetof(Vertex, tex);

		return attDesc;
	};
};
#endif