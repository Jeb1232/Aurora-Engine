#include "Shader.h"

Shader::Shader(std::string shaderName) {

	name = shaderName;
}

void Shader::SetVertexShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	ID3DBlob* tmpBlob = nullptr;

	std::wstring widestr = std::wstring(path.begin(), path.end());

	D3DReadFileToBlob((LPCWSTR)widestr.c_str(), &tmpBlob);


	m_device->CreateVertexShader(tmpBlob->GetBufferPointer(), tmpBlob->GetBufferSize(), nullptr, &vertexShader);

	delete tmpBlob;
}

void Shader::SetHullShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	ID3DBlob* tmpBlob = nullptr;

	std::wstring widestr = std::wstring(path.begin(), path.end());

	D3DReadFileToBlob((LPCWSTR)widestr.c_str(), &tmpBlob);

	m_device->CreateHullShader(tmpBlob->GetBufferPointer(), tmpBlob->GetBufferSize(), nullptr, &hullShader);

	delete tmpBlob;
}

void Shader::SetDomainShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	ID3DBlob* tmpBlob = nullptr;

	std::wstring widestr = std::wstring(path.begin(), path.end());

	D3DReadFileToBlob((LPCWSTR)widestr.c_str(), &tmpBlob);

	m_device->CreateDomainShader(tmpBlob->GetBufferPointer(), tmpBlob->GetBufferSize(), nullptr, &domainShader);

	delete tmpBlob;
}

void Shader::SetPixelShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	ID3DBlob* tmpBlob = nullptr;

	std::wstring widestr = std::wstring(path.begin(), path.end());

	D3DReadFileToBlob((LPCWSTR)widestr.c_str(), &tmpBlob);

	m_device->CreatePixelShader(tmpBlob->GetBufferPointer(), tmpBlob->GetBufferSize(), nullptr, &pixelShader);

	delete tmpBlob;
}

void Shader::SetGeometryShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	ID3DBlob* tmpBlob = nullptr;

	std::wstring widestr = std::wstring(path.begin(), path.end());

	D3DReadFileToBlob((LPCWSTR)widestr.c_str(), &tmpBlob);

	m_device->CreateGeometryShader(tmpBlob->GetBufferPointer(), tmpBlob->GetBufferSize(), nullptr, &geomShader);

	delete tmpBlob;
}

void Shader::SetComputeShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
	ID3DBlob* tmpBlob = nullptr;

	std::wstring widestr = std::wstring(path.begin(), path.end());

	D3DReadFileToBlob((LPCWSTR)widestr.c_str(), &tmpBlob);

	m_device->CreateComputeShader(tmpBlob->GetBufferPointer(), tmpBlob->GetBufferSize(), nullptr, &compShader);

	delete tmpBlob;
}

Shader::~Shader() {

}