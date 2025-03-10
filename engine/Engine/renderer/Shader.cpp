#include "Shader.h"

Shader::Shader(std::string shaderName) {

	name = shaderName;
}

void Shader::SetVertexShader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	

	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	m_device->CreateVertexShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &vertexShader);

	delete sBlob;
	sBlob = nullptr;
}

void Shader::SetHullShader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	m_device->CreateHullShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &hullShader);

	delete sBlob;
	sBlob = nullptr;
}

void Shader::SetDomainShader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	m_device->CreateDomainShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &domainShader);

	delete sBlob;
	sBlob = nullptr;
}

void Shader::SetPixelShader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	m_device->CreatePixelShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &pixelShader);

	delete sBlob;
	sBlob = nullptr;
}

void Shader::SetGeometryShader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	m_device->CreateGeometryShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &geomShader);

	delete sBlob;
	sBlob = nullptr;
}

void Shader::SetComputeShader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	m_device->CreateComputeShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &compShader);

	delete sBlob;
	sBlob = nullptr;
}

Shader::~Shader() {

}