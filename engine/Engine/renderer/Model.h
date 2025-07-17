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
#include <glm/glm.hpp>
#include<glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include"../asset/AssetManager.h"
#include"../renderer/Camera.h"



struct BoundingVolume
{
	//virtual bool isOnFrustum(const Frustum& camFrustum, const glm::vec3& transform) const = 0;

	//virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;
	/*
	bool isOnFrustum(const Frustum& camFrustum) const
	{
		return (isOnOrForwardPlane(camFrustum.leftFace) &&
			isOnOrForwardPlane(camFrustum.rightFace) &&
			isOnOrForwardPlane(camFrustum.topFace) &&
			isOnOrForwardPlane(camFrustum.bottomFace) &&
			isOnOrForwardPlane(camFrustum.nearFace) &&
			isOnOrForwardPlane(camFrustum.farFace));
	};
	*/
};

struct AABB
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	glm::vec3 extents{ 0.f, 0.f, 0.f };

	AABB(const glm::vec3& min, const glm::vec3& max) {
		center = (max + min) * 0.5f;
		extents = glm::vec3(max.x - center.x, max.y - center.y, max.z - center.z);
	}

	AABB(const glm::vec3& inCenter, float iI, float iJ, float iK) {
		center = inCenter;
		extents = glm::vec3(iI, iJ, iK);
	}
	/*
	std::array<glm::vec3, 8> getVertice() const
	{
		std::array<glm::vec3, 8> vertice;
		vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
		vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
		vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
		vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
		vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
		vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
		vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
		vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
		return vertice;
	}
	*/
	//see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	bool isOnOrForwardPlane(const Plane& plane) 
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
			extents.z * std::abs(plane.normal.z);

		return -r <= plane.getSignedDistanceToPlane(center);
	}

	bool isOnFrustum(const Frustum& camFrustum, const glm::mat4& transform) 
	{
		//Get global scale thanks to our transform

		
		glm::vec3 globalCenter{ transform * glm::vec4(center, 1.f) };

		// Scaled orientation
		glm::vec3 right = transform[0] * extents.x;
		glm::vec3 up = transform[1] * extents.y;
		glm::vec3 forward = -transform[2] * extents.z;

		float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		AABB globalAABB(globalCenter, newIi, newIj, newIk);

		return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
			globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
			globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
			globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
			globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
			globalAABB.isOnOrForwardPlane(camFrustum.farFace));

	};
};

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
		float tx, ty, tz;
		//float btx, bty, btz;
	};

	struct VertexNT {
		//glm::vec3 pos;
		//glm::vec3 normals;
		//glm::vec2 uvs;
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		//float tx, ty, tz;
		//float btx, bty, btz;
	};
	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;
		std::string material;
		std::string materialname;
		std::string materialSpec;
		std::string materialNorm;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		D3D11_BUFFER_DESC ibd = {};
		D3D11_SUBRESOURCE_DATA isd = {};
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		AABB boundingBox = AABB(glm::vec3(), glm::vec3());
	};
	std::string directory;
	Vertex test;
	//void ProcessMesh(const aiMesh* mesh, std::vector<Vertex>& mVertices, std::vector<unsigned short>& mIndices);
	//void ProcessNode(const aiScene* scene, aiNode* node);
	void LoadModel(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void LoadModelFromPAK(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void LoadModelThread(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	MaterialManager::Material setMaterial(aiMaterial* mat, aiTextureType type, std::string typeName, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	//std::vector<S_Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	HRESULT result;
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
private:
	std::thread modelThread;
};

