#ifndef INPUT_HPP
#define INPUT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <map>

namespace Input
{
	extern GLFWwindow* window;
	extern glm::vec2 mousePosition;

	struct KeyState
	{
		bool currentState = false; // state in the current frame
		bool lastState = false;    // state in the last frame
	};

	std::map<int, KeyState> keyStates;

	void InitInput(GLFWwindow* _window)
	{
		window = _window;
	}

	void UpdateInput()
	{
		for (auto& pair : keyStates)
		{
			int key = pair.first;
			pair.second.lastState = pair.second.currentState;
			pair.second.currentState = glfwGetKey(window, key) == GLFW_PRESS;
		}
	}

	bool GetKeyDown(int key)
	{
		return keyStates[key].currentState;
	}

	bool GetKeyUp(int key)
	{
		return !keyStates[key].currentState;
	}

	bool GetKeyJustPressed(int key)
	{
		return keyStates[key].currentState && !keyStates[key].lastState;
	}

	bool GetMouseButtonDown(int button)
	{
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	bool GetMouseButtonUp(int button)
	{
		return glfwGetMouseButton(window, button) == GLFW_RELEASE;
	}

	void SetCursorMode(const bool& value)
	{
		if (!value)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

GLFWwindow* Input::window;
glm::vec2 Input::mousePosition;

#endif // !INPUT_HPP