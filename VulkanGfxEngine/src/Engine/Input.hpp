#include <GLFW/glfw3.h>

#ifndef INPUT
#define INPUT

class Input {
public:
	inline static float mouse_x;
	inline static float mouse_y;

	inline static float move_x;
	inline static float move_y;

	inline static void keyPress_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods) {
		
		if (action != GLFW_PRESS && action != GLFW_REPEAT) {
			return;
		}
		
		switch (key) {
		case GLFW_KEY_W:
			move_y = 1.0f;
			break;
		case GLFW_KEY_S:
			move_y = -1.0f;
			break;
		case GLFW_KEY_A:
			move_x = 1.0f;
			break;
		case GLFW_KEY_D:
			move_x = -1.0f;
			break;
		}


	}

	inline static void mouse_callback(GLFWwindow* wnd, double x, double y) {
		mouse_x = (float)x / 1000;
		mouse_y = (float)y / 1000;
	
	}

	inline static void reset() {
		move_x = 0;
		move_y = 0;

		//mouse_x = 0;
		//mouse_y = 0;
	}
};
#endif