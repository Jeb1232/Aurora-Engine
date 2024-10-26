#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

Texture::Texture(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext)
{
	Tpath = path;

	ImageTexture = nullptr;
	ImageShaderResourceView = nullptr;
	ImageSamplerState = nullptr;

	int ImageWidth;
	int ImageHeight;
	int ImageChannels;
	int ImageDesiredChannels = 4;

	stbi_set_flip_vertically_on_load(true);

	AssetPacker packer;
	AssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", path);

	unsigned char* ImageData = stbi_load_from_memory(lFile.data, lFile.fileSize,
		&ImageWidth,
		&ImageHeight,
		&ImageChannels, ImageDesiredChannels);

	int ImagePitch = ImageWidth * 4;

	if (!ImageData) {
		std::cout << "Failed to load image: " << path << '\n';
		stbi_image_free(ImageData);
		return;
	}
	std::cout << ImageWidth + " " + ImageHeight << std::endl;
	free(lFile.data);

	// Texture


	ImageTextureDesc.Width = ImageWidth;
	ImageTextureDesc.Height = ImageHeight;
	ImageTextureDesc.MipLevels = 6;
	ImageTextureDesc.ArraySize = 1;
	ImageTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	ImageTextureDesc.SampleDesc.Count = 1;
	ImageTextureDesc.SampleDesc.Quality = 0;
	ImageTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	ImageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	ImageTextureDesc.CPUAccessFlags = 0;
	ImageTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	ImageSubresourceData.pSysMem = ImageData;
	ImageSubresourceData.SysMemPitch = sizeof(unsigned char) * ImagePitch;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = ImageTextureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 6;

	m_device->CreateTexture2D(&ImageTextureDesc,
		nullptr,
		&ImageTexture
	);

	// Sampler

	m_deviceContext->UpdateSubresource(ImageTexture, 0, nullptr, ImageData, ImageSubresourceData.SysMemPitch, 0);

	m_device->CreateShaderResourceView(ImageTexture,
		&srvDesc,
		&ImageShaderResourceView
	);
	m_deviceContext->GenerateMips(ImageShaderResourceView);

	stbi_image_free(ImageData);

	ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.MipLODBias = -2.0f;
	ImageSamplerDesc.MaxAnisotropy = 1;
	ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ImageSamplerDesc.MinLOD = -FLT_MAX;
	ImageSamplerDesc.MaxLOD = FLT_MAX;

	m_device->CreateSamplerState(&ImageSamplerDesc,
		&ImageSamplerState);
}

Texture::Texture(std::vector<const char*> paths, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext)
{
	//Tpath = paths;


	ImageTexture = nullptr;
	ImageShaderResourceView = nullptr;
	ImageSamplerState = nullptr;

	stbi_set_flip_vertically_on_load(false);

	AssetPacker packer;
	std::vector<AssetPacker::LoadedFile> lFiles;
	lFiles.push_back(packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", paths[0]));
	lFiles.push_back(packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", paths[1]));
	lFiles.push_back(packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", paths[2]));
	lFiles.push_back(packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", paths[3]));
	lFiles.push_back(packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", paths[4]));
	lFiles.push_back(packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", paths[5]));

	std::vector<LoadedImage> images;

	for (int i = 0; i < 6; i++) {
		LoadedImage tmplImage;
		images.push_back(tmplImage);
		images[i].data = stbi_load_from_memory(lFiles[i].data, lFiles[i].fileSize,
			&images[i].ImageWidth,
			&images[i].ImageHeight,
			&images[i].ImageChannels, images[i].ImageDesiredChannels);

		images[i].ImagePitch = images[i].ImageWidth * 4;

		if (!images[i].data) {
			std::cout << "Failed to load image: " << paths[i] << '\n';
			stbi_image_free(images[i].data);
			return;
		}

		free(lFiles[i].data);
	}



	ImageTextureDesc.Width = images[0].ImageWidth;
	ImageTextureDesc.Height = images[0].ImageHeight;
	ImageTextureDesc.MipLevels = 1;
	ImageTextureDesc.ArraySize = 6;
	ImageTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ImageTextureDesc.SampleDesc.Count = 1;
	ImageTextureDesc.SampleDesc.Quality = 0;
	ImageTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	ImageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ImageTextureDesc.CPUAccessFlags = 0;
	ImageTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA pData[6];

	for (int i = 0; i < 6; i++) {

		pData[i].pSysMem = images[i].data;
		pData[i].SysMemPitch = sizeof(unsigned char) * images[i].ImagePitch;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = ImageTextureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	m_device->CreateTexture2D(&ImageTextureDesc,
		&pData[0],
		&ImageTexture
	);

	m_device->CreateShaderResourceView(ImageTexture,
		&srvDesc,
		&ImageShaderResourceView
	);
	//m_deviceContext->GenerateMips(ImageShaderResourceView);

	stbi_image_free(images[0].data);

	ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.MipLODBias = 0.0f;
	ImageSamplerDesc.MaxAnisotropy = 1;
	ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ImageSamplerDesc.MinLOD = -FLT_MAX;
	ImageSamplerDesc.MaxLOD = FLT_MAX;

	m_device->CreateSamplerState(&ImageSamplerDesc,
		&ImageSamplerState);

}

Texture::~Texture()
{
	if (ImageTexture != nullptr) { ImageTexture->Release(); }
	if (ImageShaderResourceView != nullptr) { ImageShaderResourceView->Release(); }
	if (ImageSamplerState != nullptr) { ImageSamplerState->Release(); }
}