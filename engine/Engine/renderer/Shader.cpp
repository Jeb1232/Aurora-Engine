#include "Shader.h"

Shader::Shader(const char* path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext, ShaderType type) {
	D3DReadFileToBlob((LPCWSTR)path, &sBlob);

	// noob-like if statement nest
	if (type == ShaderType::Vertex) {
		m_device->CreateVertexShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &vertexShader);
	}
	else if (type == ShaderType::Pixel) {
		m_device->CreatePixelShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &pixelShader);
	}
	else if (type == ShaderType::Hull) {
		m_device->CreateHullShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &hullShader);
	}
	else if (type == ShaderType::Domain) {
		m_device->CreateDomainShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &domainShader);
	}
	else if (type == ShaderType::Geometry) {
		m_device->CreateGeometryShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &geomShader);
	}
	else if (type == ShaderType::Compute) {
		m_device->CreateComputeShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, &compShader);
	}

	if (type == ShaderType::Vertex) {
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		m_device->CreateInputLayout(layout, 3, sBlob->GetBufferPointer(), sBlob->GetBufferSize(), &inputLayout);
	}

}

Shader::~Shader() {

}