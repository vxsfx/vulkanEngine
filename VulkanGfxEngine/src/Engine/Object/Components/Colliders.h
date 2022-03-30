#pragma once
#include "Transform.h"
#include "Component.h"
#include "../../util/math.hpp"

class Collider : public Component{
public:
	Transform* transform;

	std::vector<float3>vertices;
	bool curremtlyCollided;

	static std::vector<Collider*> allColliders;

	//AVOID ignore STATIC colliders(checked with non static)

	bool isStatic;

	virtual bool hasCollided(Collider* other) = 0;

	virtual void onCollide() = 0;

	static void updateCollisions();

	Collider(bool isStatic);
};

//basic estimation for box collider
//most memory efficent(only two vertices)
class stupidCollider : public Collider {
public:
	bool hasCollided(Collider* other) override;

	void update() override;


	
};