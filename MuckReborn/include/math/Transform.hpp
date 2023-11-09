#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TRANSFORM_DEFAULT Transform::Register(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f})
#define TRANSFORM_POSITION(x, y, z) Transform::Register(glm::vec3{x, y, z}, glm::vec3{0.0f, 0.0f, 0.0f})
#define TRANSFORM_ROTATION(x, y, z) Transform::Register(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{x, y, z})

struct Transform
{
	void Translate(const glm::vec3& translation)
	{
		position += translation;
	}

	void Rotate(const glm::vec3& rotation)
	{
		this->rotation += rotation;
	}

	static Transform Register(const glm::vec3& position, const glm::vec3& rotation)
	{
		Transform transform = {};

		transform.rotationIndex = 0.01f;
		transform.position = position;
		transform.rotation = rotation;
		transform.up = glm::vec3{ 0.0f, 1.0f, 0.0f };

		return transform;
	}

	float rotationIndex;
	glm::vec3 rotation;
	glm::vec3 position;
	glm::vec3 up;
};

#endif // !TRANSFORM_HPP