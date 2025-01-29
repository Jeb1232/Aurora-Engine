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
#include"Model.h"
#include"MaterialManager.h"
#include"Light.h"
#include"../scene/Scene.h"
#include"Camera.h"
//#include"../physics/aPhysicsSystem.h"

class Renderer
{
public:
	Scene* currentActiveScene = nullptr;
	static Renderer* instance;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	struct Vertex {
		//glm::vec3 pos;
		//glm::vec3 normals;
		//glm::vec2 uvs;
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float tx, ty, tz;
	};
	struct Vertex2 {
		//glm::vec3 pos;
		//glm::vec3 normals;
		//glm::vec2 uvs;
		float x, y;
		float u, v;
	};
	__declspec(align(16))
	struct ConstantBuffer {
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 lightSpaceMatrix;
		glm::vec3 cameraPos;
		//float padding;
	};
	//__declspec(align(16))
		struct SconstantBuffer {
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 ProjectionMatrix;
		//glm::mat4 lightSpaceMatrix;
	};
	__declspec(align(16))
	struct LightCB {
		glm::vec3 position;
	    glm::vec3 direction;
		float intensity;
		glm::vec3 lightColor;
		glm::vec3 ambientColor;
		//glm::vec3 camPosition;
	};
	__declspec(align(16))
		struct PostProcessBuffer {
		glm::mat4 prevProjMatrix;
		glm::mat4 projMatrix;
		glm::vec3 camPos;
	};
	SDL_Window* window;
	HWND hWindow;
	SDL_SysWMinfo wmInfo;
	bool finishedRendering = false;
	void AURORAENGINE_API InitRenderer(int width, int height, HWND hWnd);
	void AURORAENGINE_API CreateRenderWindow(int width, int height, bool fullscreen);
	void AURORAENGINE_API RenderLoop(int width, int height);
	void AURORAENGINE_API DrawFrame(int width, int height, HWND hwnd);
	void AURORAENGINE_API EditorRenderLoop(int width, int height, HWND hwnd);
	void AURORAENGINE_API SetActiveScene(Scene* scene);
	bool init = false;
	bool edResizing = false;
private:
	void AURORAENGINE_API SDLInput();
	void AURORAENGINE_API SetD3DStates();
	void AURORAENGINE_API ThrowIfFailed(HRESULT hr);
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11RenderTargetView* m_renderTargetView2 = nullptr;
	//ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11HullShader* hullShader = nullptr;
	ID3D11DomainShader* domainShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11InputLayout* skyboxInputLayout = nullptr;
	ID3D11InputLayout* ppinputLayout = nullptr;
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11DepthStencilState* m_depthState = nullptr;
	ID3D11BlendState* m_blendState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11DepthStencilView* m_shadowDepthStencilView = nullptr;
	ID3D11Texture2D* m_backBuffer = nullptr;
	ID3D11Texture2D* m_renderTexture = nullptr;
	ID3D11Texture2D* m_renderTexture2 = nullptr;
	ID3D11ShaderResourceView* m_rtSRV = nullptr;
	ID3D11Texture2D* m_depthBuffer = nullptr;
	ID3D11Texture2D* m_depthTexture = nullptr;
	ID3D11Texture2D* m_shadowMap = nullptr;
	ID3D11Texture2D* m_shadowMap2 = nullptr;
	ID3D11ShaderResourceView* m_shadowSRV = nullptr;
	ID3D11ShaderResourceView* m_depthSRV = nullptr;
	std::vector<ConstantBuffer> constantBuffers;
	std::thread editorrenderThread;
};

