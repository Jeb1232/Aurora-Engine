#include "Terrain.h"
#include"../renderer/stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

Terrain::Terrain(int width, int depth, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	//Texture heightMapTex("hmap.png", m_device, m_deviceContext);

	int Width = 0;
	int Height = 0;
	int channels;
	unsigned char* data = stbi_load("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/Terrain_heightmap.png", &Width, &Height, &channels, 1);

	std::cout << "HEIGHTMAP WIDTH: " << Width << std::endl;
	int index = 0;
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;
	float positionX, positionZ;

	int index5 = 0;

	//int k = 0;

	//std::vector<HeightMapPixel> m_heightMap;

	//m_heightMap.resize(Width * Height);

	int height;

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	//int i = 0;
	//int j = 0;

	terrainData.vertices.resize(Width * Height);
	terrainData.indices.resize((Width - 1) * (Height - 1) * 6);

	unsigned bytesPerPixel = channels;
	int Index1 = 0;

	
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			Vertex newVertex;

			int vertIndex = i * Width + j;

			unsigned char* texel = data + (j + Width * i) * bytesPerPixel;
			// raw height at coordinate
			unsigned char y = texel[0];
			/*
			unsigned char* texelu = data + (j + 1 + width * i) * bytesPerPixel;
			unsigned char* texeld = data + (j - 1 + width * i) * bytesPerPixel;
			unsigned char* texell = data + (j + width * i - 1) * bytesPerPixel;
			unsigned char* texelr = data + (j + width * i + 1) * bytesPerPixel;

			unsigned char yu = texelu[0];
			unsigned char yd = texeld[0];
			unsigned char yl = texell[0];
			unsigned char yr = texelr[0];
			*/
			terrainData.vertices[vertIndex].x = j;
			terrainData.vertices[vertIndex].y = y * 0.25f;
			terrainData.vertices[vertIndex].z = i;
			
			glm::vec3 normalVector = glm::vec3(0.0f, 1.0f, 0.0f);
			/*
			if (j < 0 || j >= width || i < 0 || i >= width) {
				normalVector = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			else {
				normalVector = glm::normalize(glm::vec3((yu - yd), (yu - yd) * (yr - yl), (yr - yl)));
			}
			*/
			terrainData.vertices[vertIndex].nx = normalVector.x;
			terrainData.vertices[vertIndex].ny = normalVector.y;
			terrainData.vertices[vertIndex].nz = normalVector.z;

			terrainData.vertices[vertIndex].u = (float)j / (float)Width;
			terrainData.vertices[vertIndex].v = (float)i / (float)Height;

			//terrainData.vertices.push_back(newVertex);
			//terrainData.indices.push_back(Index1);

			Index1++;
		}
	}
	
	stbi_image_free(data);
	
	for (unsigned int i = 0; i < Height - 1; i++)       // for each row a.k.a. each strip
	{
		for (unsigned int j = 0; j < Width; j++)      // for each column
		{
			for (unsigned int k = 0; k < 2; k++)      // for each side of the strip
			{
				//terrainData.indices.push_back(j + Width * (i + k));
			}
		}
	}

	int Index = 0;
	for (int z = 0; z < Height - 1; z++) {
		for (int x = 0; x < Width - 1; x++) {
			//unsigned short IndexBottomLeft = z * Width + x;
			//unsigned short IndexTopLeft = (z + 1) * Width + x;
			//unsigned short IndexTopRight = (z + 1) * Width + x + 1;
			//unsigned short IndexBottomRight = z * Width + x + 1;
			unsigned short IndexTopLeft = (z * Width) + x;          // Upper left.
			unsigned short IndexTopRight = (z * Width) + x + 1;      // Upper right.
			unsigned short IndexBottomLeft = ((z + 1) * Width) + x;      // Bottom left.
			unsigned short IndexBottomRight = ((z + 1) * Width) + x + 1;  // Bottom right.

			int topLeft = z * Width + x;
			int topRight = topLeft + 1;
			int bottomLeft = topLeft + Width;
			int bottomRight = bottomLeft + 1;

			// Triangle 1
			terrainData.indices[Index++] = topLeft;
			terrainData.indices[Index++] = bottomLeft;
			terrainData.indices[Index++] = topRight;

			// Triangle 2
			terrainData.indices[Index++] = topRight;
			terrainData.indices[Index++] = bottomLeft;
			terrainData.indices[Index++] = bottomRight;

			
			//unsigned short vertexIndex = z * Width + x;

			// Add top left triangle

			//terrainData.indices[Index++] = IndexTopLeft;
			//terrainData.indices[Index++] = IndexTopRight;
			//terrainData.indices[Index++] = IndexBottomLeft;

			// Add bottom right triangle
			//terrainData.indices[Index++] = IndexBottomLeft;
			//terrainData.indices[Index++] = IndexTopRight;
			//terrainData.indices[Index++] = IndexBottomRight;
			
		}
	}
	/*
	for (int i = 0; i < Height - 1; ++i)
	{
		for (int j = 0; j < Width - 1; ++j)
		{
			// Upper triangle of the quad
			terrainData.indices.emplace_back(j + (i * Width));
			terrainData.indices.emplace_back(j + ((i + 1) * Width) + 1);
			terrainData.indices.emplace_back(j + (i * Width) + 1);

			// Lower triangle of the quad
			terrainData.indices.emplace_back(j + (i * Width));
			terrainData.indices.emplace_back(j + ((i + 1) * Width));
			terrainData.indices.emplace_back(j + ((i + 1) * Width) + 1);
		}
	}
	*/
	
	//int Index = 0;
	/*
	for (int i = 0; i < Height - 1; i++)       // for each row a.k.a. each strip
	{
		for (int j = 0; j < Width; j++)      // for each column
		{
			for (int k = 0; k < 2; k++)      // for each side of the strip
			{
				terrainData.indices.push_back(j + Width * (i + k));
			}
		}
	}
	*/
	
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
	ibd.ByteWidth = sizeof(unsigned int) * terrainData.indices.size();
	//ibd.StructureByteStride = sizeof(unsigned int);
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
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	//m_deviceContext->Draw((UINT)terrainData.vertices.size(), 0);

}