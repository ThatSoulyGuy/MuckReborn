#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "core/Logger.hpp"
#include "core/EventSystem.hpp"
#include "core/Input.hpp"
#include "util/General.hpp"

struct WindowResizeData : public IPackagable
{
	GLFWwindow* window;
	glm::ivec2 size;

	static WindowResizeData Register(GLFWwindow* window, const glm::ivec2& size)
	{
		WindowResizeData out = {};

		out.window = window;
		out.size = size;

		return out;
	}
};

void ResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	//EventSystem::DispatchEvent(EventType::WINDOW_RESIZE_EVENT, WindowResizeData::Register(window, glm::ivec2{ width, height }).Package());
}

void MouseMove(GLFWwindow* window, double xposIn, double yposIn)
{
	Input::mousePosition.x = static_cast<float>(xposIn);
	Input::mousePosition.y = static_cast<float>(yposIn);
}

struct WindowData : public IPackagable
{
    GLFWwindow* window = NULL;

	std::string title = "";
	glm::ivec2 size = {};
	glm::ivec2 position = {};

	glm::vec3 color = {};

	float deltaTime = 0.0f, currentFrame = 0.0f, lastFrame = 0.0f;
};

struct Window
{
	WindowData data;

	static Window mainWindow;

	void GenerateWindow(const std::string& title, const glm::ivec2& size, const glm::vec3& color = glm::vec3{0.0f, 0.0f, 0.0f})
	{
		Logger_FunctionStart;

		data.window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
		data.size = size;
		data.color = color;

		if (!data.window)
			std::cout << "Failed to create GLFW window" << std::endl;

		glfwMakeContextCurrent(data.window);
		glfwSetCursorPosCallback(data.window, MouseMove);
		glfwSetFramebufferSizeCallback(data.window, ResizeWindow);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			std::cout << "Failed to initialize GLAD" << std::endl;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		EventSystem::DispatchEvent(EventType::WINDOW_INIT_EVENT, NULL);

		Logger_FunctionEnd;
	}

	bool ShouldClose()
	{
		return glfwWindowShouldClose(data.window);
	}

	void UpdateColors()
	{
		data.currentFrame = glfwGetTime();
		data.deltaTime = data.currentFrame - data.lastFrame;
		data.lastFrame = data.currentFrame;

		glfwGetWindowSize(data.window, &data.size.x, &data.size.y);
		glfwGetWindowPos(data.window, &data.position.x, &data.position.y);

		glClearColor(data.color.x, data.color.y, data.color.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void UpdateBuffers()
	{
		glfwSwapBuffers(data.window);
		glfwPollEvents();
	}

	void CleanUp()
	{
		Logger_FunctionStart;

		glfwTerminate();

		EventSystem::DispatchEvent(EventType::WINDOW_CLEANUP_EVENT, NULL);

		Logger_FunctionEnd;
	}
};

Window Window::mainWindow;

#endif // !WINDOW_HPP