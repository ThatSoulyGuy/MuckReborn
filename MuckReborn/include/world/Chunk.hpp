#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "math/Noise.hpp"
#include "rendering/Renderer.hpp"
#include "util/General.hpp"

#define CHUNK_SIZE 6

struct ChunkData : IPackagable
{
	RenderableObject* object = 0;

	std::vector<Vertex> vertices = {};
	std::vector<unsigned int> indices = {};
	int indiceIndex = 0;
};

class Chunk : IPackagable
{

public:
	
	void InitalizeChunk(const glm::ivec3& position)
	{
		data.object = new RenderableObject();
		noise = new Noise(0.45, 10);

		data.object = RenderableObject::Register("Chunk(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")", {}, {}, false, false, ShaderManager::GetShader(ShaderType::CHUNK));
		data.object->data.transform.position = position;

		Rebuild();
	}

	void Rebuild()
	{
		data.vertices.clear();
		data.indices.clear();
		data.indiceIndex = 0;

		for (float x = 0; x < CHUNK_SIZE; x += 0.125f)
		{
			for (float z = 0; z < CHUNK_SIZE; z += 0.125f)
			{
				GenerateQuad({ x, 0, z });
			}
		}

		for (Vertex& vertex : data.vertices)
		{
			float noiseValue = noise->FractalNoise(CHUNK_SIZE * 4, vertex.position.x, vertex.position.z);

			vertex.position.y = noiseValue * scale + offset;
		}

		data.object->RegisterTexture(TextureManager::GetTexture("test_texture"));
		data.object->ReRegister(data.vertices, data.indices);
		data.object->GenerateRawData();

		Renderer::RegisterRenderableObject(data.object);
	}

	void GenerateQuad(const glm::vec3& position)
	{
		float height00 = GetHeight(position.x, position.z);
		float height01 = GetHeight(position.x, position.z + 0.125f);
		float height10 = GetHeight(position.x + 0.125f, position.z);
		float height11 = GetHeight(position.x + 0.125f, position.z + 0.125f);

		glm::vec3 normal = CalculateNormal(position.x, position.z);

		data.vertices.push_back(Vertex::Register({ position.x, height00, position.z }, normal, DEFAULT_COLOR, { 0.0f, 0.0f }));
		data.vertices.push_back(Vertex::Register({ position.x, height01, position.z + 0.125f }, normal, DEFAULT_COLOR, { 0.0f, 1.0f }));
		data.vertices.push_back(Vertex::Register({ position.x + 0.125f, height11, position.z + 0.125f }, normal, DEFAULT_COLOR, { 1.0f, 1.0f }));
		data.vertices.push_back(Vertex::Register({ position.x + 0.125f, height10, position.z }, normal, DEFAULT_COLOR, { 1.0f, 0.0f }));

		data.indices.push_back(data.indiceIndex);
		data.indices.push_back(1 + data.indiceIndex);
		data.indices.push_back(3 + data.indiceIndex);

		data.indices.push_back(3 + data.indiceIndex);
		data.indices.push_back(1 + data.indiceIndex);
		data.indices.push_back(2 + data.indiceIndex);

		data.indiceIndex += 4;
	}

	void CleanUp()
	{
		delete noise;

		delete this;
	}

	ChunkData data;

private:

	glm::vec3 CalculateNormal(float x, float z)
	{
		glm::vec3 tangentX = glm::vec3(0.125f, GetHeight(x + 0.125f, z) - GetHeight(x, z), 0.0f);
		glm::vec3 tangentZ = glm::vec3(0.0f, GetHeight(x, z + 0.125f) - GetHeight(x, z), 0.125f);

		glm::vec3 normal = glm::normalize(glm::cross(tangentX, tangentZ));

		return normal;
	}

	float GetHeight(float x, float z)
	{
		return noise->FractalNoise(CHUNK_SIZE * 4, x, z) * scale + offset;
	}

	Noise* noise = nullptr;
	float scale = 1.0f;
	float offset = 0.0f;

};

#endif // !CHUNK_HPP