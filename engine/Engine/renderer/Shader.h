#pragma once
#include"../dllHeader.h"
#include<iostream>
#include <chrono>
#include<d3d11.h>
//#include<dxgi.h>
#include<d3dcompiler.h>
#include<string>
class Shader
{
public:
	/*
	enum ShaderType {
		Vertex,
		Pixel,
		Hull,
		Domain,
		Geometry,
		Compute
	};
	*/
	Shader(std::string shaderName);

	void SetVertexShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void SetHullShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void SetDomainShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void SetPixelShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void SetGeometryShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void SetComputeShader(std::string path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);

	~Shader();

	std::string name;
	ID3DBlob* sBlob = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11HullShader* hullShader = nullptr;
	ID3D11DomainShader* domainShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11GeometryShader* geomShader = nullptr;
	ID3D11ComputeShader* compShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;

private:
};

