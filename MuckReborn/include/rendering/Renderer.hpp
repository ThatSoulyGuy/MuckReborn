#ifndef RENERER_HPP
#define RENERER_HPP

#define DEFAULT_COLOR glm::vec3{1.0f, 1.0f, 1.0f}

#include <vector>
#include <cstddef>
#include <map>
#include <deque>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "core/Logger.hpp"
#include "core/EventSystem.hpp"
#include "math/Transform.hpp"
#include "rendering/Camera.hpp"
#include "rendering/ShaderManager.hpp"
#include "rendering/TextureManager.hpp"
#include "util/General.hpp"

struct Vertex : public IPackagable
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 textureCoords;

	static Vertex Register(const glm::vec3& position, const glm::vec3& color, const glm::vec3& normal, const glm::vec2& textureCoords)
	{
		Vertex out = {};

		out.position = position;
		out.color = color;
		out.normal = normal;
		out.textureCoords = textureCoords;

		return out;
	}
};

enum class GLPointerType
{
	D,
	I
};

struct GLPointerCall
{
	int from = 0, to = 0, stride = 0, index = 0;
	unsigned int type = 0;
	bool normalized = true;
	void* pointer = {};
	std::string name = "";
	GLPointerType pointerType = GLPointerType::D;

	static GLPointerCall Register(int from, int to, unsigned int type, bool normalized, int stride, void* pointer, int index, const std::string& name, GLPointerType pointerType)
	{
		GLPointerCall out;

		out.from = from;
		out.to = to;
		out.stride = stride;
		out.index = index;
		out.type = type;
		out.normalized = normalized;
		out.pointer = pointer;
		out.name = name;
		out.pointerType = pointerType;

		return out;
	}
};

struct GLBufferCall
{
	std::string bind = "";
	std::string name = "";
	unsigned int type = 0, size = 0, draw = 0;
	void* pointer = {};

	static GLBufferCall Register(unsigned int type, unsigned int size, void* pointer, unsigned int draw, const std::string& bind, const std::string& name)
	{
		GLBufferCall out;

		out.bind = bind;
		out.type = type;
		out.size = size;
		out.draw = draw;
		out.pointer = pointer;
		out.name = name;

		return out;
	}
};

struct RenderableData : public IPackagable
{
	std::string name = "";
	bool advanced = false;
	ShaderObject shader = {};
	Transform transform = TRANSFORM_DEFAULT;
	std::deque<GLPointerCall> pointerCalls = {};
	std::deque<GLBufferCall> bufferCalls = {};
	std::map<std::string, Texture> textures = {};
	bool completelyReplaceDefaultGLPointerCalls = false;

	std::vector<Vertex> vertices = {};
	std::vector<unsigned int> indices = {};
	std::map<std::string, unsigned int> buffers =
	{
		{"VAO", 0},
		{"VBO", 0},
		{"EBO", 0}
	};
};

struct ShaderCall
{
	std::string objectName = "";
	std::string variableName = "";
	std::string type = "";
	void* value = {};

	static ShaderCall Register(const std::string& objectName, const std::string& variableName, const std::string& type, void* value)
	{
		ShaderCall out;

		out.objectName = objectName;
		out.variableName = variableName;
		out.type = type;
		out.value = value;

		return out;
	}
};

class RenderableObject
{

public:

	RenderableData data;

	void GenerateRawData()
	{
		data.shader.GenerateShader();

		glGenVertexArrays(1, &data.buffers["VAO"]);
		glGenBuffers(1, &data.buffers["VBO"]);
		glGenBuffers(1, &data.buffers["EBO"]);

		glBindVertexArray(data.buffers["VAO"]);

		PostGLBufferCalls();
		PostGLPointerCalls();
		
		glBindVertexArray(0);

		for (auto& [key, value] : data.textures)
			value.GenerateTexture();

		if (!data.advanced)
		{
			data.shader.Use();
			data.shader.SetUniform("texture_diffuse1", 0);
		}
	}

	void GenerateTestObject()
	{
		data.name = "testObject";

		std::vector<Vertex> vertices
		{
			Vertex::Register({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),

			Vertex::Register({0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),


			Vertex::Register({0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.0f}, {-1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.0f, 0.0f, 0.5f}, {-1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),

			Vertex::Register({0.5f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),


			Vertex::Register({0.0f, 0.5f, 0.0f}, {0.0f, -1.0f,  0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.5f}, {0.0f, -1.0f,  0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.5f}, {0.0f, -1.0f,  0.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.5f, 0.0f}, {0.0f, -1.0f,  0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),

			Vertex::Register({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f,  0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.0f, 0.5f}, {0.0f, 1.0f,  0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.0f, 0.5f}, {0.0f, 1.0f,  0.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.0f}, {0.0f, 1.0f,  0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),
		};

		std::vector<unsigned int> indices
		{
			0, 1, 3,
			1, 2, 3,

			7, 6, 5,
			7, 5, 4,

			11, 10, 9,
			11, 9, 8,

			12, 13, 15,
			13, 14, 15,

			16, 17, 19,
			17, 18, 19,

			23, 22, 21,
			23, 21, 20
		};
		
		data.advanced = false;
		data.transform = TRANSFORM_DEFAULT;
		data.shader = ShaderManager::GetShader(ShaderType::DEFAULT);
		RegisterTexture(TextureManager::GetTexture("test_texture"));

		ReRegister(vertices, indices);
		GenerateRawData();
	}

	void RequestGLPointerCall(const GLPointerCall& call)
	{
		data.pointerCalls.push_back(call);
	}

	void PostGLPointerCalls()
	{
		if (!data.completelyReplaceDefaultGLPointerCalls)
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
			glEnableVertexAttribArray(3);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		while (!data.pointerCalls.empty())
		{
			auto& call = data.pointerCalls.front();

			switch (call.pointerType)
			{

			case GLPointerType::D:

				glVertexAttribPointer(call.from, call.to, call.type, call.normalized, call.stride, call.pointer);
				break;

			case GLPointerType::I:

				glVertexAttribIPointer(call.from, call.to, call.type, call.stride, call.pointer);
				break;

			default:
				break;

			}

			glEnableVertexAttribArray(call.index);

			data.pointerCalls.pop_front();
		}
	}

	void RequestGLBufferCall(GLBufferCall call)
	{
		data.bufferCalls.push_back(call);
	}

	void PostGLBufferCalls()
	{
		if (data.bufferCalls.size() <= 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, data.buffers["VBO"]);
			glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.buffers["EBO"]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

			return;
		}

		while (!data.bufferCalls.empty())
		{
			auto& call = data.bufferCalls.front();

			if (call.bind == "VBO")
				glBindBuffer(call.type, data.buffers["VBO"]);
			else if (call.bind == "EBO")
				glBindBuffer(call.type, data.buffers["EBO"]);
			else
				throw new std::exception("call.bind is f*cked");

			glBufferData(call.type, call.size, call.pointer, call.draw);

			data.bufferCalls.pop_front();
		}
	}

	static RenderableObject* Register(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, bool advanced = false, bool completelyReplaceGLPointerCalls = false, const ShaderObject& shader = ShaderManager::GetShader(ShaderType::DEFAULT))
	{
		RenderableObject* out = new RenderableObject();

		out->data.name = name;
		out->data.shader = shader;
		out->data.vertices = vertices;
		out->data.indices = indices;
		out->data.advanced = advanced;
		out->data.completelyReplaceDefaultGLPointerCalls = completelyReplaceGLPointerCalls;

		return out;
	}

	void RegisterTexture(const Texture& texture)
	{
		data.textures.insert({ texture.name, texture });
	}

	void ReRegister(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	{
		data.vertices = vertices;
		data.indices = indices;
	}

	void CleanUp()
	{
		Logger_FunctionStart;

		glDeleteVertexArrays(1, &data.buffers["VAO"]);
		glDeleteBuffers(1, &data.buffers["VBO"]);
		glDeleteBuffers(1, &data.buffers["EBO"]);

		data.vertices.clear();
		data.indices.clear();
		data.buffers.clear();

		data.shader.CleanUp();

		Logger_FunctionEnd;

		delete this;
	}
};

namespace Renderer
{
	extern std::map<std::string, RenderableObject*> renderableObjects;
	extern std::deque<ShaderCall> shaderCalls;
	bool drawLines = false;

	void RegisterRenderableObject(RenderableObject* object)
	{
		//if (renderableObjects[object->data.name])
		//	renderableObjects[object->data.name] = object;

		renderableObjects.insert({object->data.name, object});
	}

	template<typename T>
	void RequestShaderCall(const std::string& objectName, const std::string& variableName, T value)
	{
		shaderCalls.push_back(ShaderCall::Register(objectName, variableName, typeid(T).name(), &value));
	}

	void PostShaderCalls()
	{
		while (!shaderCalls.empty())
		{
			for (auto& [key, value] : renderableObjects)
			{
				auto& call = shaderCalls.front();

				//Logger_WriteConsole(call.type, LogLevel::DEBUG);

				if (value->data.name == call.objectName)
				{
					if (call.type == "int")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<int*>(call.value));
					else if (call.type == "float")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<float*>(call.value));
					else if (call.type == "bool")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<bool*>(call.value));

					else if (call.type == "struct glm::vec<2,float,0>")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<glm::vec2*>(call.value));
					else if (call.type == "struct glm::vec<3,float,0>")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<glm::vec3*>(call.value));
					else if (call.type == "struct glm::vec<4,float,0>")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<glm::vec4*>(call.value));

					else if (call.type == "struct glm::mat<2,2,float,0>")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<glm::mat2*>(call.value));
					else if (call.type == "struct glm::mat<3,3,float,0>")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<glm::mat3*>(call.value));
					else if (call.type == "struct glm::mat<4,4,float,0>")
						value->data.shader.SetUniform(call.variableName, *reinterpret_cast<glm::mat4*>(call.value));
					else
						Logger_ThrowError("Invalid type", "Graphical issues are imminent", false);
				}

				shaderCalls.pop_front();
			}
		}
	}

	void RenderObjects(Camera camera)
	{
		for (auto& [key, value] : renderableObjects)
		{
			int count = 0;
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;

			value->data.shader.Use();

			for (auto const& [key, value1] : value->data.textures)
			{
				glActiveTexture(GL_TEXTURE0 + count);

				if (value->data.advanced)
				{
					std::string number;
					TextureType name = value1.properties.type;
					if (name == TextureType::DIFFUSE)
						number = std::to_string(diffuseNr++);
					else if (name == TextureType::SPECULAR)
						number = std::to_string(specularNr++);
					else if (name == TextureType::NORMAL)
						number = std::to_string(normalNr++);

					value->data.shader.SetUniform((TextureType2String(name) + number), count);
				}
				
				glBindTexture(GL_TEXTURE_2D, value1.textureID);

				++count;
			}

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, value->data.transform.position);
			//model = glm::rotate(model, value->data.transform.rotationIndex, value->data.transform.rotation);
			
			PostShaderCalls();

			value->data.shader.SetUniform("projection", camera.data.matrices.projection);
			value->data.shader.SetUniform("view", camera.data.matrices.view);
			value->data.shader.SetUniform("model", model);

			glBindVertexArray(value->data.buffers["VAO"]);

			if(drawLines)
				glDrawElements(GL_LINES, value->data.indices.size(), GL_UNSIGNED_INT, 0);
			else
				glDrawElements(GL_TRIANGLES, value->data.indices.size(), GL_UNSIGNED_INT, 0);

			int error = glGetError();

			if (error != GL_NO_ERROR)
				Logger_ThrowError(std::to_string(error), fmt::format("OpenGL error: {}", error), false);
		}
	}

	RenderableObject* GetRenderableObject(const std::string& name)
	{
		return renderableObjects[name];
	}

	void CleanUpObjects()
	{
		Logger_FunctionStart;

		for (auto& [key, value] : renderableObjects)
			value->CleanUp();

		renderableObjects.clear();

		EventSystem::DispatchEvent(EventType::RENDERER_CLEANUP_EVENT, NULL);

		Logger_FunctionEnd;
	}
}

std::map<std::string, RenderableObject*> Renderer::renderableObjects;
std::deque<ShaderCall> Renderer::shaderCalls;

#endif // !RENERER_HPP