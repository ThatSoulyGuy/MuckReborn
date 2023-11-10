#ifndef MODEL_HPP
#define MODEL_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "rendering/Renderer.hpp"

class Model
{

public:
    
    std::vector<Texture> loadedTextures;
    std::vector<RenderableObject*> meshes;
    bool gammaCorrection;

    void GenerateModel(const std::string& localPath, const std::string& domain = Settings::defaultDomain)
    {
        Logger_FunctionStart;

        LoadModel("assets/" + domain + "/" + localPath);

        Logger_FunctionEnd;
    }

    void CleanUp()
    {
        Logger_FunctionStart;
        Logger_FunctionEnd;
        delete this;
    }

private:
    
    void LoadModel(const std::string& path)
    {        
        Logger_FunctionStart;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Logger_ThrowError("NULL", "Unable to load model, " + std::string(importer.GetErrorString()), false);
            return;
        }

        Logger_FunctionEnd;

        ProcessBone(scene->mRootNode, scene);
    }

    void ProcessBone(aiNode* node, const aiScene* scene)
    {
        Logger_FunctionStart;

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
            meshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessBone(node->mChildren[i], scene);

        Logger_FunctionEnd;
    }

    RenderableObject* ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        Logger_FunctionStart;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex = Vertex::Register({}, {}, {}, {});

            vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            
            if (mesh->mTextureCoords[0])
                vertex.textureCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            else
                vertex.textureCoords = {0.0f, 0.0f};

            vertices.push_back(vertex);
        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        std::vector<Texture> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        
        std::vector<Texture> normalMaps = LoadTextures(material, aiTextureType_HEIGHT, TextureType::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        RenderableObject* object = RenderableObject::Register("mesh", vertices, indices);

        for (Texture& texture : loadedTextures)
            object->RegisterTexture(texture);

        object->GenerateRawData();
        Renderer::RegisterRenderableObject(object);

        Logger_FunctionEnd;

        return object;
    }

    std::vector<Texture> LoadTextures(aiMaterial* material, aiTextureType type, TextureType textureType)
    {
        Logger_FunctionStart;

        std::vector<Texture> textures;

        for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString path;
            material->GetTexture(type, i, &path);
            
            bool skip = false;
            for (Texture& texture : loadedTextures)
            {
                if (std::strcmp(texture.path.c_str(), path.C_Str()) == 0)
                {
                    textures.push_back(texture);
                    skip = true;
                    break;
                }
            }

            if (!skip)
                loadedTextures.push_back(Texture::Register(path.C_Str(), material->GetName().C_Str(), TEXTURE_PROPERTIES_TYPE(textureType)));
        }

        Logger_FunctionEnd;

        return textures;
    }
};

#endif // !MODEL_HPP