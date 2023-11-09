#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/Settings.hpp"
#include "util/General.hpp"

enum class ShaderType
{
	DEFAULT
};

struct ShaderObject
{
	std::string vertexData = "", fragmentData = "";
	std::string vertexPath = "", fragmentPath = "";
	ShaderType type = ShaderType::DEFAULT;

	unsigned int shaderProgram;

	unsigned int CompileShader(GLenum type, const std::string& source) const  
	{
		unsigned int shader = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) 
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		return shader;
	}

	void LinkProgram(unsigned int vertexShader, unsigned int fragmentShader) 
	{
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		int success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if (!success) 
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
	}

	void GenerateShader()
	{
		unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexData);
		unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentData);

		LinkProgram(vertexShader, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Use() 
	{
		glUseProgram(shaderProgram);
	}

	template<typename T>
	void SetUniform(const std::string& name, const T& value) const { };

	template<>
	void SetUniform<bool>(const std::string& name, const bool& value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
	}

	template<>
	void SetUniform<int>(const std::string& name, const int& value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	template<>
	void SetUniform<float>(const std::string& name, const float& value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	template<>
	void SetUniform<glm::vec2>(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	template<>
	void SetUniform<glm::vec3>(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	template<>
	void SetUniform<glm::vec4>(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	template<>
	void SetUniform<glm::mat2>(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	template<>
	void SetUniform<glm::mat3>(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	template<>
	void SetUniform<glm::mat4>(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void SetUniform(const std::string& name, float x, float y) const
	{
		SetUniform<glm::vec2>(name, glm::vec2(x, y));
	}

	void SetUniform(const std::string& name, float x, float y, float z) const
	{
		SetUniform<glm::vec3>(name, glm::vec3(x, y, z));
	}

	void SetUniform(const std::string& name, float x, float y, float z, float w) const
	{
		SetUniform<glm::vec4>(name, glm::vec4(x, y, z, w));
	}

	void CleanUp()
	{
		glDeleteProgram(shaderProgram);
	}

	static ShaderObject Register(const std::string& path, ShaderType type, const std::string& domain = Settings::defaultDomain)
	{
		ShaderObject out = {};

		out.type = type;

		out.vertexPath = "assets/" + domain + "/" + path + "Vertex.glsl";
		out.fragmentPath = "assets/" + domain + "/" + path + "Fragment.glsl";

		out.vertexData = LoadFile(out.vertexPath);
		out.fragmentData = LoadFile(out.fragmentPath);

		return out;
	}
};

namespace ShaderManager
{
	extern std::vector<ShaderObject> registeredShaders;

	void RegisterShader(const ShaderObject& object)
	{
		registeredShaders.push_back(object);
	}

	ShaderObject GetShader(ShaderType type)
	{
		for (ShaderObject object : registeredShaders)
		{
			if (object.type == type)
				return object;
		}
	}
}

std::vector<ShaderObject> ShaderManager::registeredShaders;

#endif // !SHADER_MANAGER_HPP