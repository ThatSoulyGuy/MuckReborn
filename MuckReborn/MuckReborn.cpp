#include "core/Logger.hpp"
#include "core/EventSystem.hpp"
#include "core/Settings.hpp"
#include "core/Window.hpp"
#include "gameplay/Player.hpp"
#include "rendering/LightingManager.hpp"
#include "rendering/Model.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/TextureManager.hpp"
#include "world/Chunk.hpp"

Player player;
Window window;
Chunk* model;

int main()
{
	Logger_Init();

	Settings::InitGLFW();
	ShaderManager::RegisterShader(ShaderObject::Register("shaders/default", ShaderType::DEFAULT));
	ShaderManager::RegisterShader(ShaderObject::Register("shaders/chunk", ShaderType::CHUNK));
	TextureManager::RegisterTexture(Texture::Register("textures/test_image.png", "test_texture"));
	TextureManager::RegisterTexture(Texture::Register("textures/terrain.png", "terrain_atlas"));
	TextureManager::RegisterTexture(Texture::Register("models/Tisch_t.png", "Tisch_t"));

	window.GenerateWindow("Muck Reborn* 0.1.0", glm::ivec2{ 750, 450 }, glm::vec3{ 0.0f, 0.34f, 0.51f });
	Window::mainWindow = window;

	EventSystem::DispatchEvent(EventType::MR_PRE_INIT_EVENT, NULL);

	LightingManager::SetDirectionalLight(DirectionalLight::Register({ 0.45994705f, -0.88781524f, 0.015258028f }, { 0.05f, 0.05f, 0.05f }, { 0.4f, 0.4f, 0.4f }, { 0.5f, 0.5f, 0.5f }));
	LightingManager::AddPointLight("light1", PointLight::Register({0.0f, 5.0f, 0.0f}, { 0.05f, 0.05f, 0.05f }, { 0.8f, 0.8f, 0.8f }, { 1.0f, 1.0f, 1.0f }));

	Input::InitInput(window.data.window);

	player.InitalizePlayer({ 0.0f, 0.0f, 0.0f });
	
	model = new Chunk();
	model->InitalizeChunk({0, 0, 0});

	Logger_WriteConsole("Hello, World!", LogLevel::INFO);

	EventSystem::DispatchEvent(EventType::MR_INIT_EVENT, NULL);

	while (!window.ShouldClose())
	{
		window.UpdateColors();
		
		Input::UpdateInput();
		player.Update();

		LightingManager::PostLightingInstructions(player.data.camera);
		Renderer::RenderObjects(player.data.camera);

		window.UpdateBuffers();

		Window::mainWindow = window;
	}

	window.CleanUp();
	model->CleanUp();
	Renderer::CleanUpObjects();

	EventSystem::DispatchEvent(EventType::MR_CLEANUP_EVENT, NULL);

	Logger_CleanUp();

	return 0;
}