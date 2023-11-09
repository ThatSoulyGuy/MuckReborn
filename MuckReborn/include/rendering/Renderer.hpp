#ifndef RENERER_HPP
#define RENERER_HPP

#define DEFAULT_COLOR glm::vec3{1.0f, 1.0f, 1.0f}

#include <vector>
#include <cstddef>
#include <map>
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
	glm::vec2 textureCoords;

	static Vertex Register(const glm::vec3& position, const glm::vec3& color, const glm::vec2& textureCoords)
	{
		Vertex out = {};

		out.position = position;
		out.color = color;
		out.textureCoords = textureCoords;

		return out;
	}
};

struct RenderableData : public IPackagable
{
	std::string name = "";
	bool advanced = false;
	ShaderObject shader;
	Transform transform = TRANSFORM_DEFAULT;
	std::map<std::string, Texture> textures = {};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::map<std::string, unsigned int> buffers =
	{
		{"VAO", 0},
		{"VBO", 0},
		{"EBO", 0}
	};
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

		glBindBuffer(GL_ARRAY_BUFFER, data.buffers["VBO"]);
		glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.buffers["EBO"]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
			Vertex::Register({0.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.0f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),

			Vertex::Register({0.0f, 0.0f, 0.5f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.5f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.5f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.5f}, DEFAULT_COLOR, {0.0f, 1.0f}),


			Vertex::Register({0.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.5f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.0f, 0.0f, 0.5f}, DEFAULT_COLOR, {0.0f, 1.0f}),

			Vertex::Register({0.5f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.0f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.5f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.5f}, DEFAULT_COLOR, {0.0f, 1.0f}),


			Vertex::Register({0.0f, 0.5f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.5f, 0.5f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.5f, 0.5f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.5f, 0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),

			Vertex::Register({0.0f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 0.0f}),
			Vertex::Register({0.0f, 0.0f, 0.5f}, DEFAULT_COLOR, {1.0f, 0.0f}),
			Vertex::Register({0.5f, 0.0f, 0.5f}, DEFAULT_COLOR, {1.0f, 1.0f}),
			Vertex::Register({0.5f, 0.0f, 0.0f}, DEFAULT_COLOR, {0.0f, 1.0f}),
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

	static RenderableObject* Register(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, bool advanced = false, const ShaderObject& shader = ShaderManager::GetShader(ShaderType::DEFAULT))
	{
		RenderableObject* out = new RenderableObject();

		out->data.name = name;
		out->data.shader = shader;
		out->data.vertices = vertices;
		out->data.indices = indices;
		out->data.advanced = advanced;

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
	bool drawLines = false;

	void RegisterRenderableObject(RenderableObject* object)
	{
		//if (renderableObjects[object->data.name])
		//	renderableObjects[object->data.name] = object;

		renderableObjects.insert({object->data.name, object});
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

#endif // !RENERER_HPP