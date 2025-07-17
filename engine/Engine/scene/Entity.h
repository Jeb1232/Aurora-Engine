#pragma once
#include<iostream>
#include<vector>
#include"../dllHeader.h"
#include"../renderer/Shader.h"
//#include"../renderer/Renderer.h"
#include"../audio/AudioSource.h"
#include"../renderer/Model.h"
#include"../renderer/Light.h"
#include"../renderer/MaterialManager.h"
#include"../renderer/Camera.h"
#include"../core/Component.h"



class Entity
{
public:
	MaterialManager matManager;

	std::vector<Component> components;

	Entity* parent = nullptr;
	std::vector<Entity*> children;
	enum EntityType {
		NONE,
		MODEL,
		AUDIOSOURCE,
		LIGHT
	};

	struct Material {
		std::string name;
		ID3D11ShaderResourceView* diffuse = nullptr;
		ID3D11ShaderResourceView* normal = nullptr;
		ID3D11ShaderResourceView* specular = nullptr;
	};

	struct Transform {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};

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

	__declspec(align(16))
		struct ConstantBuffer {
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::vec3 cameraPos;
	};

	Transform transform;

	glm::mat4 Matmodel = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first
	glm::mat4 Matview = glm::mat4(1.0f);
	glm::mat4 Matprojection = glm::mat4(1.0f);

	Entity(const char* Ename, EntityType entType);
	~Entity();

	EntityType type;
	const char* name;
	Model model;
	//AudioSource source;
	Light light;
	bool useMaterials = false;
	bool texturesLoaded = false;
	bool frustumCulling = true;
	//std::vector<ID3D11ShaderResourceView*> textures;
	std::vector<Material> materials;
	//std::vector<ID3D11ShaderResourceView*> texturesSpec;
	std::string texPath;
	void AURORAENGINE_API setModel(Model Imodel);
	void AURORAENGINE_API setASource(AudioSource aSource);
	void AURORAENGINE_API setLSource(Light lSource);
	void AURORAENGINE_API setPosition(glm::vec3 pos);
	void AURORAENGINE_API setRotation(glm::vec3 rot);
	void AURORAENGINE_API setScale(glm::vec3 scale);
	void AURORAENGINE_API Update();
	void AURORAENGINE_API UpdateMatrixes(glm::mat4 view, glm::mat4 projection, glm::vec3 camPos);
	void AURORAENGINE_API DrawModel(ID3D11DeviceContext* m_deviceContext, ID3D11Device* m_device, Frustum cameraFrustum, bool frustumCull);
private:
	glm::vec3 camPosition;
};

