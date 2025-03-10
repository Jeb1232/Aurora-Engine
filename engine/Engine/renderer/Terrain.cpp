#include "Terrain.h"
#include"../renderer/stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

Terrain::Terrain(int width, int depth, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	//Texture heightMapTex("hmap.png", m_device, m_deviceContext);

	int Width = 0;
	int Height = 0;
	int channels = 4;
	unsigned char* data = stbi_load("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/Terrain_heightmap.png", &Width, &Height, &channels, 0);

	std::cout << "HEIGHTMAP WIDTH: " << Width << std::endl;
	int index = 0;
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;
	float positionX, positionZ;

	int index5 = 0;

	//int k = 0;

	std::vector<HeightMapPixel> heightMap;

	heightMap.resize(width * depth);

	int height;

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	int i = 0;
	int j = 0;

	//terrainData.vertices.resize((width - 1) * (depth - 1) * 8);
	//terrainData.indices.resize((width - 1) * (depth - 1) * 8);

	for (i = 0; i < width; i++) {
		for (j = 0; j < depth; j++) {
			Vertex newVertex;

			unsigned char* texel = data + (j + width * i) * 4;
			// raw height at coordinate
			unsigned char y = texel[0];

			newVertex.x = i;
			newVertex.y = y * 0.5f;
			newVertex.z = j;

			newVertex.nx = 0.0f;
			newVertex.ny = 1.0f;
			newVertex.nz = 0.0f;

			newVertex.u = (float)i / (float)width;
			newVertex.v = (float)j / (float)depth;

			terrainData.vertices.push_back(newVertex);
		}
	}

	for (unsigned int i2 = 0; i2 < depth - 1; i2++)       // for each row a.k.a. each strip
	{
		for (unsigned int j2 = 0; j2 < width; j2++)      // for each column
		{
			for (unsigned int k2 = 0; k2 < 2; k2++)      // for each side of the strip
			{
				terrainData.indices.push_back(j2 + width * (i2 + k2));
			}
		}
	}

	
	ID3D11Buffer* vertexBuffer = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(terrainData.vertices.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);
	terrainData.vertexBuffer = vertexBuffer;
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = terrainData.vertices.data();

	ID3D11Buffer* indexBuffer = nullptr;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(unsigned short) * terrainData.indices.size();
	ibd.StructureByteStride = sizeof(unsigned short);
	terrainData.indexBuffer = indexBuffer;
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = terrainData.indices.data();

	terrainData.vertexBufferDesc = vertexBufferDesc;
	terrainData.vertexData = vertexData;
	terrainData.ibd = ibd;
	terrainData.isd = isd;

	m_device->CreateBuffer(&terrainData.vertexBufferDesc, &terrainData.vertexData, &terrainData.vertexBuffer);
	m_device->CreateBuffer(&terrainData.ibd, &terrainData.isd, &terrainData.indexBuffer);
	delete[] vertexBuffer;
	delete[] indexBuffer;
}

Terrain::~Terrain() {

}

void Terrain::DrawTerrain(ConstBuffer cBuf, ID3D11VertexShader* m_vertexShader, ID3D11PixelShader* m_pixelShader, ID3D11DeviceContext* m_deviceContext, ID3D11Device* m_device) {
	//Texture heightMap("hmap.png", m_device, m_deviceContext);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_deviceContext->IASetVertexBuffers(0, 1, &terrainData.vertexBuffer, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(terrainData.indexBuffer, DXGI_FORMAT_R16_UINT, 0u);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11Buffer* constantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cBuf);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cBuf;

	m_device->CreateBuffer(&cbd, &csd, &constantBuffer);

	m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	m_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	//m_deviceContext->VSSetShaderResources(0, 1, &heightMap.ImageShaderResourceView);
	//m_deviceContext->VSSetSamplers(0, 1, &heightMap.ImageSamplerState);

	m_deviceContext->DrawIndexed((UINT)terrainData.indices.size(), 0, 0);

}