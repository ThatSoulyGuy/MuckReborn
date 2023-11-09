#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <map>

namespace Settings
{
	extern std::string defaultDomain;
	extern std::map<std::string, void*> randomData;
	bool debug = true;

	void InitGLFW()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 16);

		defaultDomain = "muckreborn";
	}

	template<typename T>
	T* ConvertRandom(const std::string& name)
	{
		return (T*)randomData[name];
	}
}

std::string Settings::defaultDomain;
std::map<std::string, void*> Settings::randomData;

#endif // !SETTINGS_HPP