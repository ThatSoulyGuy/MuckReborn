#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "math/Noise.hpp"
#include "rendering/Renderer.hpp"
#include "util/General.hpp"

#define CHUNK_SIZE 16

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
		noise = new Noise(0.45, 0.85);

		data.object = RenderableObject::Register("Chunk(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")", {}, {});
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
			float noiseValue = noise->FractalNoise(12, vertex.position.x, vertex.position.z);

			vertex.position.y = noiseValue * scale + offset;
		}

		data.object->RegisterTexture(TextureManager::GetTexture("test_texture"));
		data.object->ReRegister(data.vertices, data.indices);
		data.object->GenerateRawData();

		Renderer::RegisterRenderableObject(data.object);
	}

	void GenerateQuad(const glm::vec3& position)
	{
		data.vertices.push_back(Vertex::Register({ 0.0f + position.x, 0.125f + position.y, 0.0f + position.z }, DEFAULT_COLOR, { 0.0f, 0.0f }));
		data.vertices.push_back(Vertex::Register({ 0.0f + position.x, 0.125f + position.y, 0.125f + position.z }, DEFAULT_COLOR, { 0.0f, 1.0f }));
		data.vertices.push_back(Vertex::Register({ 0.125f + position.x, 0.125f + position.y, 0.125f + position.z }, DEFAULT_COLOR, { 1.0f, 0.0f }));
		data.vertices.push_back(Vertex::Register({ 0.125f + position.x, 0.125f + position.y, 0.0f + position.z }, DEFAULT_COLOR, { 1.0f, 1.0f }));

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

	Noise* noise = nullptr;
	float scale = 1.0f;
	float offset = 0.0f;

};

#endif // !CHUNK_HPP