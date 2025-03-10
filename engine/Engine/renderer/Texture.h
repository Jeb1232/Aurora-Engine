#pragma once
#include"../dllHeader.h"
#include<iostream>
#include <chrono>
#include<d3d11.h>
//#include<dxgi.h>
#include<d3dcompiler.h>
#include<thread>
#include<exception>
#include<fstream>
#include<vector>
//#include"Renderer.h"
#include"../file/asset_packer.h"
class Texture
{
public:
	struct LoadedImage {
		int ImagePitch;
		int ImageWidth;
		int ImageHeight;
		int ImageChannels;
		int ImageDesiredChannels = 4;
		unsigned char* data;
	};
	Texture(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	Texture(std::vector<const char*> paths, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	~Texture();
	unsigned char* iData;
	void AURORAENGINE_API getPixel(size_t imageWidth, size_t x, size_t y, int r, int g, int b, int a);

	const char* Tpath;
	D3D11_TEXTURE2D_DESC ImageTextureDesc = {};
	D3D11_SUBRESOURCE_DATA ImageSubresourceData = {};
	ID3D11Texture2D* ImageTexture = nullptr;
	ID3D11ShaderResourceView* ImageShaderResourceView = nullptr;
	ID3D11SamplerState* ImageSamplerState = nullptr;
	D3D11_SAMPLER_DESC ImageSamplerDesc = {};

private:
};

