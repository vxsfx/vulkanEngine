#include "Transform.h"

Transform::Transform(std::string giveName, float3 pos, float3 rotations) : Component(giveName)
{

	position = pos;
	rotation = rotations;
}

void Transform::update() {
	//don nothig
}

void Transform::rotateZ(float angle) {
	rotation.z = angle;
}