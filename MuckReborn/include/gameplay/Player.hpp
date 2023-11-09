#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "rendering/Camera.hpp"
#include "rendering/Renderer.hpp"

struct PlayerData
{
	Transform transform;
	Camera camera;
};

struct Player
{
	PlayerData data;

	void InitalizePlayer(const glm::vec3& position)
	{
		Logger_FunctionStart;

		data.transform = TRANSFORM_POSITION(position.x, position.y, position.z);
		data.camera.InitalizeCamera(position);

		Logger_FunctionEnd;
	}

	void Update()
	{
		data.camera.data.transform.position = data.transform.position;
		data.camera.Update();

		UpdateMouseMovement();
		UpdateDebugControls();
		UpdateBlockEditing();
		UpdateMovement();
	}

	void UpdateBlockEditing()
	{
		if (Input::GetMouseButtonDown(0))
		{
			
		}
	}

	void UpdateDebugControls()
	{
		if (Input::GetKeyJustPressed(GLFW_KEY_L))
		{
			if (Renderer::drawLines)
				Renderer::drawLines = false;
			else
				Renderer::drawLines = true;
		}
	}

	void UpdateMovement()
	{
		float velocity = data.camera.data.movementSpeed * Window::mainWindow.data.deltaTime;

		if (Input::GetKeyDown(GLFW_KEY_W))
			data.transform.position += data.camera.data.transform.rotation * velocity;

		if (Input::GetKeyDown(GLFW_KEY_A))
			data.transform.position -= data.camera.data.right * velocity;

		if (Input::GetKeyDown(GLFW_KEY_S))
			data.transform.position -= data.camera.data.transform.rotation * velocity;

		if (Input::GetKeyDown(GLFW_KEY_D))
			data.transform.position += data.camera.data.right * velocity;

		data.camera.UpdateCameraVectors();
	}

	void UpdateMouseMovement()
	{
		float xOffset = Input::mousePosition.x - data.camera.data.lastX;
		float yOffset = data.camera.data.lastY - Input::mousePosition.y;
		data.camera.data.lastX = Input::mousePosition.x;
		data.camera.data.lastY = Input::mousePosition.y;

		xOffset *= data.camera.data.mouseSensitivity;
		yOffset *= data.camera.data.mouseSensitivity;

		data.camera.data.yaw += xOffset;
		data.camera.data.pitch += yOffset;

		if (data.camera.data.pitch > 89.0f)
			data.camera.data.pitch = 89.0f;
		if (data.camera.data.pitch < -89.0f)
			data.camera.data.pitch = -89.0f;

		data.camera.UpdateCameraVectors();
	}
};

#endif // !PLAYER_HPP