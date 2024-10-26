#include "Renderer.h"
#include"Texture.h"
#include"../scene/Entity.h"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0;
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

void Renderer::ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        // Set a breakpoint on this line to catch Win32 API errors.
        //std::cout << com_exception(hr).what() << std::endl;
        MessageBox(hWindow, com_exception(hr).what(), "Engine Error", MB_ICONWARNING | MB_OK);
    }
}

void Renderer::InitRenderer(int width, int height, HWND hWnd)
{
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[1]
    {
        D3D_FEATURE_LEVEL_11_0
    };

    //MessageBox(window, "example engine error window", "Engine Error", MB_ICONWARNING | MB_OK);

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.OutputWindow = hWnd;
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
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    ID3D11DepthStencilState* pDSState;
    m_device->CreateDepthStencilState(&dsDesc, &pDSState);
    m_deviceContext->OMSetDepthStencilState(pDSState, 1u);

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
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
    

    //m_backBuffer->Release();
    init = true;
}

float timeAng = 0;

void Renderer::RenderLoop(int width, int height)
{
    if (!init) { return; }


    MaterialManager matManager;

    MaterialManager::Material mat_1;
    MaterialManager::Material mat_2;
    MaterialManager::Material mat_3;

    mat_1.name = "mat_1";
    mat_2.name = "mat_2";
    mat_3.name = "mat_3";
    mat_1.diffuse = new Texture("concretefloor.jpg", m_device, m_deviceContext);
    mat_2.diffuse = new Texture("brickwall.jpg", m_device, m_deviceContext);
    mat_3.diffuse = new Texture("white.jpg", m_device, m_deviceContext);
    matManager.AddMaterial(mat_1);
    matManager.AddMaterial(mat_2);
    matManager.AddMaterial(mat_3);

    //Model lModel(m_device);
    //lModel.LoadModel("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/sea-keep-lonely-watcher/source/Stronghold.fbx");
    //Model lModel;

    ID3D11VertexShader* skyVertexShader = nullptr;
    ID3D11PixelShader* skyPixelShader = nullptr;
    

    Entity map0("map0", Entity::EntityType::MODEL);
    Entity map("map", Entity::EntityType::MODEL);
    Entity entity2("Thing", Entity::EntityType::MODEL);
    Entity skyCube("Skybox", Entity::EntityType::MODEL);
    ID3DBlob* vsBlob;
    ID3DBlob* hsBlob;
    ID3DBlob* dsBlob;
    ID3DBlob* psBlob;
    ID3DBlob* skyVS;
    ID3DBlob* skyPS;
    map0.matManager = matManager;
    map.matManager = matManager;
    skyCube.matManager = matManager;
    map0.model.matManager = matManager;
    map.model.matManager = matManager;
    skyCube.model.matManager = matManager;
    map0.model.LoadModelFromPAK("map0.obj", m_device, m_deviceContext);
    map.model.LoadModelFromPAK("map.obj", m_device, m_deviceContext);
    skyCube.model.LoadModelFromPAK("cube.obj", m_device, m_deviceContext);
    skyCube.model.loadMaterials = false;
    currentActiveScene->AddEntity(&map0);
    currentActiveScene->AddEntity(&map);
    //currentActiveScene->AddEntity(&skyCube);

    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/vertexShader.cso", &vsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/tessellationHullShader.cso", &hsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/tessellationDomainShader.cso", &dsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/pixelShader.cso", &psBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/skyboxVertexShader.cso", &skyVS);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/skyboxPixelShader.cso", &skyPS);

    m_device->CreateVertexShader(skyVS->GetBufferPointer(), skyVS->GetBufferSize(), nullptr, &skyVertexShader);
    m_device->CreatePixelShader(skyPS->GetBufferPointer(), skyPS->GetBufferSize(), nullptr, &skyPixelShader);

    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    m_device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &hullShader);
    m_device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &domainShader);
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);


    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_device->CreateInputLayout(layout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    m_device->CreateInputLayout(layout, 3, skyVS->GetBufferPointer(), skyVS->GetBufferSize(), &skyboxInputLayout);

    

    SetD3DStates();

    auto viewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float)width, (float)height,0,1);
    m_deviceContext->RSSetViewports(1, &viewport);

    std::vector<const char*> skyFaces;
    skyFaces.push_back("posx.jpg");
    skyFaces.push_back("negx.jpg");
    skyFaces.push_back("posy.jpg");
    skyFaces.push_back("negy.jpg");
    skyFaces.push_back("posz.jpg");
    skyFaces.push_back("negz.jpg");

    Texture* diffuse = new Texture("brickwall.jpg", m_device, m_deviceContext);
    Texture* heightMap = new Texture("Terrain005_4K.png", m_device, m_deviceContext);
    Texture* specular = new Texture("white.jpg", m_device, m_deviceContext);
    Texture* skybox = new Texture(skyFaces, m_device, m_deviceContext);
    //Texture* textures = { {"diffuse.jpg", m_device, m_deviceContext},{"specular.jpg", m_device, m_deviceContext} };
    //textures[0] = diffuse;
    //textures[1] = specular;

    //m_deviceContext->IASetIndexBuffer(lModel.meshes[0].indexBuffer, DXGI_FORMAT_R16_UINT, 0u);
    
    float clearColor[] = { 0.25f,0.5f,1,1 };
    glm::mat4 model = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float radius = 10.0f;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    //Texture* tex = nullptr;
    //std::vector<Texture*> loaded_tex;
    //diffuse->ImageShaderResourceView
    
    m_deviceContext->PSSetShaderResources(0, 1, &diffuse->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(0, 1, &diffuse->ImageSamplerState);

    m_deviceContext->PSSetShaderResources(20, 1, &skybox->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(15, 1, &skybox->ImageSamplerState);


    m_deviceContext->PSSetShaderResources(1, 1, &specular->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(1, 1, &specular->ImageSamplerState);

    float tme = 0;
    while (!finishedRendering)
    {
        SDLInput();

        m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
        m_deviceContext->RSSetState(m_rasterizerState);
        m_deviceContext->OMSetBlendState(m_blendState, NULL, 0xffffffff);

        // matrix stuff under this
        float camX = std::sin(tme) * radius;
        float camZ = std::cos(tme) * radius;
        //view = glm::lookAtRH(glm::vec3(camX, 1.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        view = camera.GetViewMatrix();
        projection = glm::perspectiveRH_ZO(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 10000.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));


        ConstantBuffer scb = {
        model,
        glm::mat4(glm::mat3(camera.GetViewMatrix())),
        projection,
        camera.Position
        };


        ID3D11Buffer* skyConstantBuffer;
        D3D11_BUFFER_DESC scbd;
        scbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        scbd.Usage = D3D11_USAGE_DYNAMIC;
        scbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        scbd.MiscFlags = 0u;
        scbd.ByteWidth = sizeof(scb);
        scbd.StructureByteStride = 0u;
        D3D11_SUBRESOURCE_DATA scsd = {};
        scsd.pSysMem = &scb;

        m_device->CreateBuffer(&scbd, &scsd, &skyConstantBuffer);
        m_deviceContext->VSSetConstantBuffers(0, 1, &skyConstantBuffer);

        //D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
        //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

        m_deviceContext->IASetInputLayout(skyboxInputLayout);
        //m_deviceContext->HSSetShader(nullptr, nullptr, 0);
        //m_deviceContext->DSSetShader(nullptr, nullptr, 0);
        m_deviceContext->VSSetShader(skyVertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(skyPixelShader, nullptr, 0);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        skyCube.DrawModel(m_deviceContext, m_device);

        //m_deviceContext->DSSetShaderResources(0, 1, &heightMap->ImageShaderResourceView);
        //m_deviceContext->DSSetSamplers(0, 1, &heightMap->ImageSamplerState);
        map0.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        map0.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        map.setPosition(glm::vec3(4.0f, -1.0f, 3.5f));
        map.setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
        map.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        //entity.UpdateMatrixes(view, projection);

        Light playerLight;

        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        for (int i = 0; i < currentActiveScene->sceneObjects.size(); i++) {
            currentActiveScene->sceneObjects[i]->UpdateMatrixes(view, projection, camera.Position);



            ConstantBuffer cb = {
                currentActiveScene->sceneObjects[i]->Matmodel,
                view,
                projection,
                camera.Position
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
            m_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
            //m_deviceContext->DSSetConstantBuffers(0, 1, &constantBuffer);
            //m_deviceContext->HSSetConstantBuffers(0, 1, &constantBuffer);


            playerLight.setPosition(camera.Position);
            playerLight.setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));

            playerLight.UpdateLightBuffer(m_device, m_deviceContext);


            //m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
            //D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
            //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
            //m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

            m_deviceContext->IASetInputLayout(inputLayout);
            m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
            //m_deviceContext->HSSetShader(hullShader, nullptr, 0);
            //m_deviceContext->DSSetShader(domainShader, nullptr, 0);
            m_deviceContext->PSSetShader(pixelShader, nullptr, 0);


            //Rendering stuff under this

            currentActiveScene->sceneObjects[i]->Update();
            currentActiveScene->sceneObjects[i]->DrawModel(m_deviceContext, m_device);
        }

        m_swapChain->Present(1, 0);

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = ((NOW - LAST) / (float)SDL_GetPerformanceFrequency());
        tme += deltaTime;
        std::cout << 1.0f/deltaTime << '\n';
        //constantBufferL->Release();
    }
    //vertexBuffer->Release();
    vertexShader->Release();
    pixelShader->Release();
    m_renderTargetView->Release();
    SDL_Quit();
}

void Renderer::SetActiveScene(Scene* scene) {
    currentActiveScene = scene;
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

    InitRenderer(width, height, (HWND)wmInfo.info.win.window);

    RenderLoop(width, height);
}

bool Edshouldberendering = true;

void Renderer::DrawFrame(int width, int height, HWND hwnd) {
    Edshouldberendering = false;
    if (editorrenderThread.joinable()) {
        editorrenderThread.join();
    }
    if (!edResizing) {
        editorrenderThread = std::thread(&Renderer::EditorRenderLoop, this, width, height, hwnd);
        edResizing = true;
    }
    edResizing = false;
    //EditorRenderLoop(width, height);
}

void Renderer::EditorRenderLoop(int width, int height,  HWND hwnd) {
    InitRenderer(width, height, hwnd);
    if (!init) { Edshouldberendering = false; return; }
    edResizing = false;
    //Model lModel(m_device);
    //lModel.LoadModel("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/sea-keep-lonely-watcher/source/Stronghold.fbx");
    Model lModel;
    lModel.LoadModelFromPAK("backpack.obj", m_device, m_deviceContext);
    ThrowIfFailed(lModel.result);
    

    ID3DBlob* vsBlob;
    ID3DBlob* hsBlob;
    ID3DBlob* dsBlob;
    ID3DBlob* psBlob;

    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/vertexShader.cso", &vsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/tessellationHullShader.cso", &hsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/tessellationDomainShader.cso", &dsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/pixelShader.cso", &psBlob);

    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    m_device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &hullShader);
    m_device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &domainShader);
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_device->CreateInputLayout(layout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);



    SetD3DStates();

    auto viewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float)width, (float)height);
    m_deviceContext->RSSetViewports(1, &viewport);

    Texture* diffuse = new Texture("diffuse.jpg", m_device, m_deviceContext);
    Texture* heightMap = new Texture("hmap.png", m_device, m_deviceContext);
    Texture* specular = new Texture("specular.jpg", m_device, m_deviceContext);
    //Texture* textures = { {"diffuse.jpg", m_device, m_deviceContext},{"specular.jpg", m_device, m_deviceContext} };
    //textures[0] = diffuse;
    //textures[1] = specular;

    //m_deviceContext->IASetIndexBuffer(lModel.meshes[0].indexBuffer, DXGI_FORMAT_R16_UINT, 0u);

    float clearColor[] = { 0.25f,0.5f,1,1 };
    glm::mat4 model = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float radius = 10.0f;


    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    //Texture* tex = nullptr;
    //std::vector<Texture*> loaded_tex;
    //m_deviceContext->VSSetShaderResources(0, 1, &heightMap->ImageShaderResourceView);
    //m_deviceContext->VSSetSamplers(0, 1, &heightMap->ImageSamplerState);
    m_deviceContext->PSSetShaderResources(0, 1, &diffuse->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(0, 1, &diffuse->ImageSamplerState);
    m_deviceContext->PSSetShaderResources(1, 1, &specular->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(1, 1, &specular->ImageSamplerState);

    float tme = 0;
    Edshouldberendering = true;
    while (Edshouldberendering) {
        //SDLInput();

        m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

        // matrix stuff under this
        float camX = std::sin(tme) * radius;
        float camZ = std::cos(tme) * radius;
        //view = glm::lookAtRH(glm::vec3(camX, 1.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        view = camera.GetViewMatrix();
        projection = glm::perspectiveRH_ZO(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 10000.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        ConstantBuffer cb = {
            model,
            view,
            projection,
            camera.Position
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
        m_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);


        glm::vec3 directionL(0.0f, -1.0f, -1.0f);

        LightCB lcb = {
            directionL,
        };

        ID3D11Buffer* constantBufferL;
        D3D11_BUFFER_DESC cbdl;
        cbdl.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbdl.Usage = D3D11_USAGE_DYNAMIC;
        cbdl.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbdl.MiscFlags = 0u;
        cbdl.ByteWidth = sizeof(lcb);
        cbdl.StructureByteStride = 0u;
        D3D11_SUBRESOURCE_DATA lcsd = {};
        lcsd.pSysMem = &lcb;
        m_device->CreateBuffer(&cbdl, &lcsd, &constantBufferL);
        //m_deviceContext->UpdateSubresource(constantBufferL, 0, 0, &lcb, 0, 0);
        m_deviceContext->PSSetConstantBuffers(0, 1, &constantBufferL);

        m_deviceContext->RSSetState(m_rasterizerState);
        m_deviceContext->OMSetBlendState(m_blendState, NULL, 0xffffffff);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
        //D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_deviceContext->IASetInputLayout(inputLayout);
        m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
        //m_deviceContext->HSSetShader(hullShader, nullptr, 0);
        //m_deviceContext->DSSetShader(domainShader, nullptr, 0);
        m_deviceContext->PSSetShader(pixelShader, nullptr, 0);

        //m_deviceContext->PSSetShaderResources(0, 1, &diffuse->ImageShaderResourceView);
        //m_deviceContext->PSSetSamplers(0, 1, &diffuse->ImageSamplerState);

        //Rendering stuff under this
        for (int i = 0; i < lModel.meshes.size(); i++)
        {
            UINT stride = sizeof(Vertex);
            UINT offset = 0;

            m_deviceContext->IASetVertexBuffers(0, 1, &lModel.meshes[i].vertexBuffer, &stride, &offset);
            m_deviceContext->IASetIndexBuffer(lModel.meshes[i].indexBuffer, DXGI_FORMAT_R16_UINT, 0u);
            /*
            for (int t = 0; t < lModel.meshes[i].diffuseMaps.size(); t++) {
                //std::string tmpPath = std::string(lModel.meshes[i].diffuseMaps[t].path);
                //std::cout << tmpPath << " " << i << '\n';
                //tex = new Texture(tmpPath.c_str(), m_device);
                if (std::string(loaded_tex[i + t]->Tpath).find('/') != std::string::npos) {
                    m_deviceContext->PSSetShaderResources(0, 1, &loaded_tex[i + t]->ImageShaderResourceView);
                    m_deviceContext->PSSetSamplers(0, 1, &loaded_tex[i + t]->ImageSamplerState);
                }
                else if (std::string(tex->Tpath).find('/') == std::string::npos) {
                    //delete[] tex;
                }
            }
            */
            m_deviceContext->DrawIndexed((UINT)lModel.meshes[i].indices.size(), 0u, 0u);
        }

        m_swapChain->Present(1, 0);
    }
    vertexShader->Release();
    pixelShader->Release();
    //m_renderTargetView->Release();
    //SDL_Quit();
}

bool firstMouse = true;


float xlastPos = 0;
float ylastPos = 0;
float xPos = 0;
float yPos = 0;
bool w, s, a, d;

void Renderer::SDLInput()
{
    SDL_Event event;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if (w) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (s) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (a) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (d) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) {
            finishedRendering = true;
            break;
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
            finishedRendering = true;
            break;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                finishedRendering = true;
                break;
            }
            if (event.key.keysym.sym == SDLK_w) {
                w = true;
            }
            if (event.key.keysym.sym == SDLK_s) {
                s = true;
            }
            if (event.key.keysym.sym == SDLK_a) {
                a = true;
            }
            if (event.key.keysym.sym == SDLK_d) {
                d = true;
            }
        }
        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_w) {
                w = false;
            }
            if (event.key.keysym.sym == SDLK_s) {
                s = false;
            }
            if (event.key.keysym.sym == SDLK_a) {
                a = false;
            }
            if (event.key.keysym.sym == SDLK_d) {
                d = false;
            }
        }
        if (event.type == SDL_MOUSEMOTION) {
            xPos += event.motion.xrel;
            yPos += event.motion.yrel;

            if (firstMouse) {
                xlastPos = xPos;
                ylastPos = yPos; 
                firstMouse = false;
            }
            
            float xOffset = xPos - xlastPos;
            float yOffset = ylastPos - yPos;

            xlastPos = xPos;
            ylastPos = yPos;

            camera.ProcessMouseMovement(xOffset, yOffset, true);
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window))
        {

        }
    }
}