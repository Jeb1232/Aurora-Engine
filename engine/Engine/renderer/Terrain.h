#pragma once
#include"../dllHeader.h"
#include"Texture.h"
#include"Model.h"
#include"Shader.h"
class Terrain
{
public:

	struct ConstBuffer {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 camPos;
		float padding;
	};
	struct Vertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	struct HeightMapPixel {
		float x, y, z;
	};

	//std::vector<Vertex> quad;

	struct TerrainData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		D3D11_SUBRESOURCE_DATA vertexData = {};
		D3D11_BUFFER_DESC ibd = {};
		D3D11_SUBRESOURCE_DATA isd = {};
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
	};

	TerrainData terrainData;

	Texture* m_heightMap = nullptr;
	Terrain(int width, int depth, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	~Terrain();

	void AURORAENGINE_API DrawTerrain(ConstBuffer cBuf, ID3D11VertexShader* m_vertexShader, ID3D11PixelShader* m_pixelShader, ID3D11DeviceContext* m_deviceContext, ID3D11Device* m_device);

private:
};

