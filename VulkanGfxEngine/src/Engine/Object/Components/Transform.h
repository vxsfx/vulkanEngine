#pragma once

#include "Component.h"
#include "../../util/math.hpp"

class Transform : public Component {
public:
	float3 position;
	float3 rotation;

	Transform(std::string giveName, float3 pos, float3 rotations);

	void update() override;

	void move(float3 dir);
	void rotate(float3 rotations);
	void rotateAxis(float3 axis, float angle);

	void rotateZ(float angle);
};