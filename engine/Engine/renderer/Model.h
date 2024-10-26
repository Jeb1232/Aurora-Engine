#pragma once
#include"../dllHeader.h"
//#include"Renderer.h"
#include"Texture.h"
#include"MaterialManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include"../file/asset_packer.h"
class Model
{
public:
	~Model();
	bool loadMaterials = true;
	MaterialManager matManager;
	struct Vertex {
		//glm::vec3 pos;
		//glm::vec3 normals;
		//glm::vec2 uvs;
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;
		std::string material;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		D3D11_BUFFER_DESC ibd = {};
		D3D11_SUBRESOURCE_DATA isd = {};
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;

	};
	std::string directory;
	Vertex test;
	//void ProcessMesh(const aiMesh* mesh, std::vector<Vertex>& mVertices, std::vector<unsigned short>& mIndices);
	//void ProcessNode(const aiScene* scene, aiNode* node);
	void LoadModel(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void LoadModelFromPAK(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	MaterialManager::Material setMaterial(aiMaterial* mat, aiTextureType type, std::string typeName, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	//std::vector<S_Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	HRESULT result;
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
private:
};

