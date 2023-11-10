#ifndef LIGHTING_MANAGER_HPP
#define LIGHTING_MANAGER_HPP

#include <string>
#include <map>
#include <type_traits>
#include <glm/glm.hpp>
#include "core/Logger.hpp"
#include "rendering/Camera.hpp"
#include "rendering/Renderer.hpp"

struct DirectionalLight
{
    glm::vec3 direction = { 0.0f, 0.0f, 0.0f };

    glm::vec3 ambient = { 0.05f, 0.05f, 0.05f };
    glm::vec3 diffuse = { 0.4f, 0.4f, 0.4f };
    glm::vec3 specular = { 0.5f, 0.5f, 0.5f };

    static DirectionalLight Register(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
    {
        return { direction, ambient, diffuse, specular };
    }
};


struct PointLight
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};

    float constant = 0.09f;
    float linear = 1.0f;
    float quadratic = 0.032f;

    glm::vec3 ambient = { 0.05f, 0.05f, 0.05f };
    glm::vec3 diffuse = { 0.8f, 0.8f, 0.8f };
    glm::vec3 specular = { 1.0f, 1.0f, 1.0f };

    static PointLight Register(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant = 0.09, float linear = 1.0, float quadratic = 0.032)
    {
        return { position, constant, linear, quadratic, ambient, diffuse, specular };
    }
};

struct SpotLight
{
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 direction = { 0.0f, 0.0f, 0.0f };

    float constant = 0.09f;
    float linear = 1.0f;
    float quadratic = 0.032f;
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(15.0f));

    glm::vec3 ambient = { 0.0f, 0.0f, 0.0f };
    glm::vec3 diffuse = { 1.0f, 1.0f, 1.0f };
    glm::vec3 specular = { 1.0f, 1.0f, 1.0f };

    static SpotLight Register(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant = 0.09, float linear = 1.0, float quadratic = 0.032, float cutOff = glm::cos(glm::radians(12.5f)), float outerCutOff = glm::cos(glm::radians(15.0f)))
    {
        return { position, direction, constant, linear, quadratic, cutOff, outerCutOff, ambient, diffuse, specular };
    }
};

struct LightingInstructions
{
    std::string name;
    std::string valueType;
    void* value;

    template<typename T>
    static LightingInstructions Register(const std::string& name, T value)
    {
        return { name, std::string(typeid(T).name()), &value };
    }
};

namespace LightingManager
{
    extern DirectionalLight directional;
    extern std::map<std::string, PointLight> pointLights;
    extern std::map<std::string, SpotLight> spotLights;

    DirectionalLight& SetDirectionalLight(const DirectionalLight& light)
    {
        directional = light;

        return directional;
    }

    PointLight& AddPointLight(const std::string& name, const PointLight& light)
    {
        if (pointLights.size() >= 10)
            Logger_ThrowError("Overflow", "'pointLights' size can only be <= 10!", true);
            
        pointLights.insert({ name, light });

        return pointLights[name];
    }

    SpotLight& AddSpotLight(const std::string& name, const SpotLight& light)
    {
        if (pointLights.size() >= 10)
            Logger_ThrowError("Overflow", "'spotLights' size can only be <= 10!", true);

        spotLights.insert({ name, light });

        return spotLights[name];
    }

    void PostLightingInstructions(Camera camera)
    {
        for (auto& [key, object] : Renderer::renderableObjects)
        {
            object->data.shaders["default"].SetUniform("viewPos", camera.data.transform.position);

            object->data.shaders["default"].SetUniform("dirLight.direction", directional.direction);
            object->data.shaders["default"].SetUniform("dirLight.ambient", directional.ambient);
            object->data.shaders["default"].SetUniform("dirLight.diffuse", directional.diffuse);
            object->data.shaders["default"].SetUniform("dirLight.specular", directional.specular);

            if (!pointLights.empty())
            {
                int plCount = 0;
                for (auto& [key, value] : pointLights)
                {
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].position", value.position);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].ambient", value.ambient);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].diffuse", value.diffuse);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].specular", value.specular);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].constant", value.constant);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].linear", value.linear);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].quadratic", value.quadratic);
                    object->data.shaders["default"].SetUniform("pointLights[" + std::to_string(plCount) + "].isActive", true);

                    plCount++;
                }
            }

            if (!spotLights.empty())
            {
                int slCount = 0;
                for (auto& [key, value] : spotLights)
                {
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].position", value.position);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].ambient", value.ambient);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].diffuse", value.diffuse);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].specular", value.specular);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].constant", value.constant);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].linear", value.linear);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].quadratic", value.quadratic);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].cutOff", value.cutOff);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].outurCutOff", value.outerCutOff);
                    object->data.shaders["default"].SetUniform("spotLights[" + std::to_string(slCount) + "].isActive", true);

                    slCount++;
                }
            }

            object->data.shaders["default"].SetUniform("material.specular", glm::vec3{ 1.0f, 1.0f, 1.0f });
            object->data.shaders["default"].SetUniform("material.diffuse", glm::vec3{ 1.0f, 1.0f, 1.0f });
            object->data.shaders["default"].SetUniform("material.shininess", 32.0f);
        }
    }
}

DirectionalLight LightingManager::directional;
std::map<std::string, PointLight> LightingManager::pointLights;
std::map<std::string, SpotLight> LightingManager::spotLights;

#endif // !LIGHTING_MANAGER_HPP