#include "Renderer.h"


class com_exception : public std::exception
{
public:
    com_exception(HRESULT hr) : result(hr) {}

    const char* what() const noexcept override
    {
        static char s_str[64] = {};
        sprintf_s(s_str, "Failure with HRESULT of %08X",
            static_cast<unsigned int>(result));
        return s_str;
    }

private:
    HRESULT result;
};

void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        // Set a breakpoint on this line to catch Win32 API errors.
        std::cout << com_exception(hr).what() << std::endl;
    }
}

void Renderer::InitRenderer() 
{
    HWND window = (HWND)wmInfo.info.win.window;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[1]
    {
        D3D_FEATURE_LEVEL_11_0
    };

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    
    HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, &featureLevel, &m_deviceContext);
    ThrowIfFailed(result);

    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);
    m_device->CreateRenderTargetView(m_backBuffer, nullptr, &m_renderTargetView);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    ID3D11DepthStencilState* pDSState;
    m_device->CreateDepthStencilState(&dsDesc, &pDSState);
    m_deviceContext->OMSetDepthStencilState(pDSState, 1u);

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = 1280u;
    descDepth.Height = 720U;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    m_device->CreateTexture2D(&descDepth, nullptr, &m_depthBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    m_device->CreateDepthStencilView(m_depthBuffer, &descDSV, &m_depthStencilView);

    m_deviceContext->OMSetRenderTargets(1u, &m_renderTargetView, m_depthStencilView);

    m_backBuffer->Release();
    init = true;
}

float timeAng = 0;

void Renderer::RenderLoop(int width, int height)
{
    if (!init) { return; }
   
    ID3DBlob* vsBlob;
    ID3DBlob* psBlob;

    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/vertexShader.cso", &vsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/pixelShader.cso", &psBlob);

    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);

    Vertex vertices[]{
        -1,-1,1,0,0,
        0,1,0,1,0,
        1,-1,0,0,1,
    };
    auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
    D3D11_SUBRESOURCE_DATA vertexData = { 0 };
    vertexData.pSysMem = vertices;

    SetD3DStates();

    auto viewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float)width, (float)height);
    m_deviceContext->RSSetViewports(1, &viewport);

    m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::transpose(model);
    float radius = 10.0f;
    view = glm::lookAtRH(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspectiveRH_ZO(glm::radians(60.0f), (float)width / (float)height, 0.1f, 1000.0f);

    ConstantBuffer cb = {
        model,
        view,
        projection
    };

    ID3D11Buffer* constantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    m_device->CreateBuffer(&cbd, &csd, &constantBuffer);
    m_deviceContext->VSSetConstantBuffers(0u, 1u, &constantBuffer);
    
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    float deltaTime = 0;

    float clearColor[] = { 0.25f,0.5f,1,1 };
    while (!finishedRendering)
    {
        SDLInput();

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = ((NOW - LAST) / (float)SDL_GetPerformanceFrequency());

        m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

        m_deviceContext->RSSetState(m_rasterizerState);
        m_deviceContext->OMSetBlendState(m_blendState, NULL, 0xffffffff);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);

        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_deviceContext->IASetInputLayout(inputLayout);
        m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(pixelShader, nullptr, 0);

        //Rendering stuff under this
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        m_deviceContext->Draw(3, 0);
        
        m_swapChain->Present(1, 0);
    }
    //vertexBuffer->Release();
    //vertexShader->Release();
    //pixelShader->Release();
    //m_renderTargetView->Release();
    SDL_Quit();
}

void Renderer::SetD3DStates()
{
    auto rasterizerDesc = CD3D11_RASTERIZER_DESC(
        D3D11_FILL_SOLID,
        D3D11_CULL_NONE,
        false,
        0, 0, 0, 0,
        false, false, false);
    m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);

    // Blend state
    auto blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
    m_device->CreateBlendState(&blendDesc, &m_blendState);
}

void Renderer::CreateRenderWindow(int width, int height, bool fullscreen) 
{
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!fullscreen)
    {
        window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    }
    else if (fullscreen)
    {
        window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    window = SDL_CreateWindow("Render Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return;
    }

    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);

    InitRenderer();

    RenderLoop(width, height);
}

void Renderer::SDLInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            finishedRendering = true;
        break;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            finishedRendering = true;
        break;
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                finishedRendering = true;
                break;
            }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window))
        {

        }
    }
}