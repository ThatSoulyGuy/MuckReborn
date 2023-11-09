#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/Input.hpp"
#include "core/Settings.hpp"
#include "math/Transform.hpp"
#include "util/General.hpp"

struct CameraMatrices : public IPackagable
{
	glm::mat4 projection;
	glm::mat4 view;

	static CameraMatrices Register(const glm::mat4& projection, const glm::mat4& view)
	{
		CameraMatrices out = {};

		out.projection = projection;
		out.view = view;

		return out;
	}
};

struct CameraData : public IPackagable
{
	CameraMatrices matrices;
	Transform transform;
	glm::vec3 right;
	glm::vec3 worldUp;

	float pitch;
	float yaw;

	float movementSpeed;
	float mouseSensitivity;

	float fov;
	float nearPlane;
	float farPlane;

	float lastX, lastY;
};

struct Camera
{
	CameraData data;

	void InitalizeCamera(const glm::vec3& position, float fov = 45.0f, float nearPlane = 0.01f, float farPlane = 100.0f, float pitch = 0.0f, float yaw = -90.0f)
	{
		Logger_FunctionStart;

		Input::SetCursorMode(false);

		data.transform = TRANSFORM_POSITION(position.x, position.y, position.z);
		data.transform.rotation = glm::vec3{ 0.0f, 0.0f, -1.0f };

		data.movementSpeed = 2.5f;
		data.mouseSensitivity = 0.1f;

		data.worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		data.pitch = pitch;
		data.yaw = yaw;

		data.fov = fov;
		data.nearPlane = nearPlane;
		data.farPlane = farPlane;

		data.lastX = static_cast<float>(Window::mainWindow.data.size.x) / static_cast<float>(2);
		data.lastY = static_cast<float>(Window::mainWindow.data.size.y) / static_cast<float>(2);

		UpdateCameraVectors();

		Logger_FunctionEnd;
	}

	void Update()
	{
		UpdateCameraVectors();
	}

	void UpdateCameraVectors() 
	{
		glm::vec3 front = {};
		front.x = cos(glm::radians(data.yaw)) * cos(glm::radians(data.pitch));
		front.y = sin(glm::radians(data.pitch));
		front.z = sin(glm::radians(data.yaw)) * cos(glm::radians(data.pitch));
		data.transform.rotation = glm::normalize(front);

		data.right = glm::normalize(glm::cross(data.transform.rotation, data.worldUp));
		data.transform.up = glm::normalize(glm::cross(data.right, data.transform.rotation));

		data.matrices = CameraMatrices::Register(glm::perspective(glm::radians(data.fov), (float)Window::mainWindow.data.size.x / (float)Window::mainWindow.data.size.y, data.nearPlane, data.farPlane), glm::lookAt(data.transform.position, data.transform.position + data.transform.rotation, data.transform.up));
	}
};

#endif // !CAMERA_HPP