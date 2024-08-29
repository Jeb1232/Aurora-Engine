#pragma once
#include"../dllHeader.h"
#include<iostream>
#include <chrono>
#include<d3d11.h>
//#include<dxgi.h>
#include<d3dcompiler.h>
#include<thread>
#include"SDL.h"
#include"SDL_syswm.h"
#include<exception>
#include<fstream>
#include<vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include<glm/gtx/matrix_major_storage.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>

class Renderer
{
public:
	struct Vertex {
		float x, y;
		float r, g, b;
	};

	struct ConstantBuffer {
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};
	SDL_Window* window;
	SDL_SysWMinfo wmInfo;
	bool finishedRendering = false;
	void AURORAENGINE_API InitRenderer();
	void AURORAENGINE_API CreateRenderWindow(int width, int height, bool fullscreen);
	void AURORAENGINE_API RenderLoop(int width, int height);
private:
	void AURORAENGINE_API SDLInput();
	void AURORAENGINE_API SetD3DStates();
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthState = nullptr;
	ID3D11BlendState* m_blendState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_backBuffer = nullptr;
	ID3D11Texture2D* m_depthBuffer = nullptr;
	bool init = false;
};

