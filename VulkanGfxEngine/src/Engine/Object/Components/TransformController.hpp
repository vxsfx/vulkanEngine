#include "Transform.h"

#include "../../Input.hpp"

#ifndef CONT
#include <chrono>
#define CONT

class Controller : public Component {
public:

	Transform* transform;
	
	inline static std::chrono::steady_clock::time_point start;

	
	inline Controller(std::string givename, Transform* tf) : Component(givename) {
		transform = tf;
		start = std::chrono::high_resolution_clock::now();
	};

	inline void update() override {
		static const float3 f = {0.0f, 0.0f, 1.0f};

		auto end = std::chrono::high_resolution_clock::now();

	    float diff = std::chrono::duration<float, std::chrono::seconds::period>(end - start).count();
		
		static float last_y_rot = 400;

		float forward = Input::move_y * diff;
		float side = Input::move_x * diff;

		float3 in = { side, 0.0f, forward };
		float3 dir = mat4::rotateY(transform->rotation.y) * in;


		transform->position.z -= dir.z;//Input::move_y * diff;
		transform->position.x -= dir.x; //Input::move_x* diff;

		transform->rotation.y += last_y_rot - Input::mouse_x;
		last_y_rot = Input::mouse_x;

		//reset Input
		Input::reset();

		start = std::chrono::high_resolution_clock::now();
	}
};
#endif