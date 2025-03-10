#include "Renderer.h"
#include"Texture.h"
#include"../scene/Entity.h"
#include"../audio/AudioDevice.h"

Camera camera(glm::vec3(10.0f, -1.8f, 0.0f));
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
        MessageBox(hWindow, com_exception(hr).what(), "Engine Error", MB_ICONERROR | MB_OK);
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

    D3D11_TEXTURE2D_DESC texDesc;
    //ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
    //DXGI_FORMAT_R16G16B16A16_FLOAT
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.CPUAccessFlags = 0;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.MiscFlags = 0;
    texDesc.MipLevels = 1;
    m_device->CreateTexture2D(&texDesc, nullptr, &m_renderTexture);
    m_device->CreateTexture2D(&texDesc, nullptr, &m_renderTexture2);
    //m_device->CreateTexture2D(&texDesc, nullptr, &m_backBuffer);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    m_device->CreateShaderResourceView(m_renderTexture, &srvDesc, &m_rtSRV);

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = texDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc2;
    renderTargetViewDesc2.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    renderTargetViewDesc2.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc2.Texture2D.MipSlice = 0;

    m_device->CreateRenderTargetView(m_renderTexture2, &renderTargetViewDesc, &m_renderTargetView);
    //m_device->CreateRenderTargetView(m_backBuffer, &renderTargetViewDesc, &m_renderTargetView);
    //m_device->CreateRenderTargetView(m_backBuffer, &renderTargetViewDesc2, &m_renderTargetView2);

    //m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);

    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);

    m_device->CreateRenderTargetView(m_backBuffer, nullptr, &m_renderTargetView2);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.StencilEnable = FALSE;
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
    descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    m_device->CreateTexture2D(&descDepth, nullptr, &m_depthBuffer);
    m_device->CreateTexture2D(&descDepth, nullptr, &m_depthTexture);
    
    D3D11_TEXTURE2D_DESC shadowMapDesc;
    //ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
    shadowMapDesc.Width = 2048;
    shadowMapDesc.Height = 2048;
    shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowMapDesc.ArraySize = 1;
    shadowMapDesc.MipLevels = 1;
    shadowMapDesc.CPUAccessFlags = 0;
    shadowMapDesc.MiscFlags = 0;
    shadowMapDesc.SampleDesc.Count = 1;
    shadowMapDesc.SampleDesc.Quality = 0;
    shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;

    //creating a texture
    m_device->CreateTexture2D(&shadowMapDesc, nullptr, &m_shadowMap);

    //creating a texture
    //m_device->CreateTexture2D(&shadowMapDesc, nullptr, &m_shadowMap2);
    //std::cout << sizeof(glm::vec3) << std::endl;
    //description for depth stencil view

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    m_device->CreateDepthStencilView(m_depthBuffer, &descDSV, &m_depthStencilView);


    D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc;
    //ZeroMemory(&shadowSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    depthSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    depthSRVDesc.Texture2D.MipLevels = 1;
    depthSRVDesc.Texture2D.MostDetailedMip = 0;
    depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    m_device->CreateShaderResourceView(m_depthTexture, &depthSRVDesc, &m_depthSRV);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    if (FAILED(m_device->CreateDepthStencilView(m_shadowMap, &depthStencilViewDesc, &m_shadowDepthStencilView))) {
        MessageBox(hWindow, "Failed to create shadow depth buffer!", "Engine Error", MB_ICONWARNING | MB_OK);
    }

    //creating a shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC shadowSRVDesc;
    //ZeroMemory(&shadowSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    shadowSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shadowSRVDesc.Texture2D.MipLevels = 1;
    shadowSRVDesc.Texture2D.MostDetailedMip = 0;
    shadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    m_device->CreateShaderResourceView(m_shadowMap, &shadowSRVDesc, &m_shadowSRV);

    m_deviceContext->OMSetRenderTargets(1u, &m_renderTargetView, m_depthStencilView);
    

    //m_backBuffer->Release();
    init = true;
}

float timeAng = 0;

void RenderShadows() {

}

void Renderer::RenderLoop(int width, int height)
{
    if (!init) { return; }
    
    cPhysicsSystem physicsSystem;
    JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));
    floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

    // Create the shape
    JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    JPH::ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

    // Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
    JPH::BodyCreationSettings floor_settings(floor_shape, JPH::RVec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::RVec3(0.0F, 200.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);



    Rigidbody planeBody(floor_settings);
    Rigidbody sphereBody(sphere_settings);


    physicsSystem.AddRigidbody(planeBody);
    physicsSystem.AddRigidbody(sphereBody);
    
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

    //ID3D11VertexShader* skyVertexShader = nullptr;
    //ID3D11PixelShader* skyPixelShader = nullptr;
    //ID3D11VertexShader* ppVertexShader = nullptr;
    //ID3D11PixelShader* ppPixelShader = nullptr;
    //ID3D11VertexShader* shadowVertexShader = nullptr;
    ID3D11PixelShader* blurPixelShader = nullptr;

    ID3D11VertexShader* terrainVertexShader = nullptr;
    ID3D11PixelShader* terrainPixelShader = nullptr;
    
    Entity entity2("Thing", Entity::EntityType::MODEL);
    Entity map("map", Entity::EntityType::MODEL);
    Entity Bistro("Bistro", Entity::EntityType::MODEL);
    Entity skyCube("Skybox", Entity::EntityType::MODEL);
    Entity physCube("PhysicsCube", Entity::EntityType::MODEL);
    Entity mask("maskman", Entity::EntityType::MODEL);
    Entity quad("quad", Entity::EntityType::MODEL);
    Entity quad3("quad3", Entity::EntityType::MODEL);
    ID3DBlob* vsBlob;
    ID3DBlob* SvsBlob;
    ID3DBlob* hsBlob;
    ID3DBlob* dsBlob;
    ID3DBlob* psBlob;
    ID3DBlob* skyVS;
    ID3DBlob* skyPS;
    ID3DBlob* ppVS;
    ID3DBlob* ppPS;
    ID3DBlob* blurPS;

    ID3DBlob* tVS;
    ID3DBlob* tPS;

    skyCube.matManager = matManager;
    skyCube.model.matManager = matManager;
    //map.model.LoadModel("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/map0.obj", m_device, m_deviceContext);
    //map.frustumCulling = false;
    //map0.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    //map.useMaterials = true;
    //entity2.model.LoadModelFromPAK("50x50Plane.obj", m_device, m_deviceContext);
    mask.model.LoadModelFromPAK("mental.obj", m_device, m_deviceContext);
    //quad.model.LoadModelFromPAK("quad2.obj", m_device, m_deviceContext);
    Bistro.model.LoadModel("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/BistroExterior.fbx", m_device, m_deviceContext);
    quad3.model.LoadModel("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/quad3.obj", m_device, m_deviceContext);
    quad3.useMaterials = false;
    quad3.setPosition(glm::vec3(-6.0f, 3.0f, -2.0f));
    //quad3.setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    Bistro.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    Bistro.setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    Bistro.setScale(glm::vec3(0.01f, 0.01f, 0.01f));
    Bistro.useMaterials = true;
    skyCube.model.LoadModelFromPAK("cube.obj", m_device, m_deviceContext);
    skyCube.model.loadMaterials = false;
    physCube.model.LoadModelFromPAK("cube.obj", m_device, m_deviceContext);
    physCube.model.loadMaterials = false;
    //currentActiveScene->AddEntity(&map);
    //currentActiveScene->AddEntity(&entity2);
    //currentActiveScene->AddEntity(&mask);
    currentActiveScene->AddEntity(&quad3);
    currentActiveScene->AddEntity(&Bistro);
    //currentActiveScene->AddEntity(&physCube);

    Shader pbrShader("PBR");
    Shader skyboxShader("Skybox");
    Shader postprocessShader("PostProcessing");
    Shader shadowShader("Shadow");

    shadowShader.SetVertexShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/ShadowVertexShader.cso", m_device, m_deviceContext);
    
    postprocessShader.SetVertexShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/PPvertexShader.cso", m_device, m_deviceContext);
    postprocessShader.SetPixelShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/PPpixelShader.cso", m_device, m_deviceContext);

    skyboxShader.SetVertexShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/skyboxVertexShader.cso", m_device, m_deviceContext);
    skyboxShader.SetPixelShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/skyboxPixelShader.cso", m_device, m_deviceContext);

    pbrShader.SetVertexShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/vertexShader.cso", m_device, m_deviceContext);
    pbrShader.SetPixelShader("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/pixelShader.cso", m_device, m_deviceContext);

    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/vertexShader.cso", &vsBlob);

    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/ShadowVertexShader.cso", &SvsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/tessellationHullShader.cso", &hsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/tessellationDomainShader.cso", &dsBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/pixelShader.cso", &psBlob);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/skyboxVertexShader.cso", &skyVS);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/skyboxPixelShader.cso", &skyPS);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/PPvertexShader.cso", &ppVS);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/PPpixelShader.cso", &ppPS);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/Blur.cso", &blurPS);

    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/TerrainVS.cso", &tVS);
    D3DReadFileToBlob(L"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/shaders/TerrainPS.cso", &tPS);

    m_device->CreateVertexShader(tVS->GetBufferPointer(), tVS->GetBufferSize(), nullptr, &terrainVertexShader);
    m_device->CreatePixelShader(tPS->GetBufferPointer(), tPS->GetBufferSize(), nullptr, &terrainPixelShader);

    //m_device->CreateVertexShader(skyVS->GetBufferPointer(), skyVS->GetBufferSize(), nullptr, &skyVertexShader);
    //m_device->CreatePixelShader(skyPS->GetBufferPointer(), skyPS->GetBufferSize(), nullptr, &skyPixelShader);

    //m_device->CreateVertexShader(ppVS->GetBufferPointer(), ppVS->GetBufferSize(), nullptr, &ppVertexShader);
   //m_device->CreatePixelShader(ppPS->GetBufferPointer(), ppPS->GetBufferSize(), nullptr, &ppPixelShader);
    m_device->CreatePixelShader(blurPS->GetBufferPointer(), blurPS->GetBufferSize(), nullptr, &blurPixelShader);

    //m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    //m_device->CreateVertexShader(SvsBlob->GetBufferPointer(), SvsBlob->GetBufferSize(), nullptr, &shadowVertexShader);
    m_device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &hullShader);
    m_device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &domainShader);
    //m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);


    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    D3D11_INPUT_ELEMENT_DESC layout2[] = {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    D3D11_INPUT_ELEMENT_DESC pplayout[] = {
        {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_device->CreateInputLayout(layout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    m_device->CreateInputLayout(layout2, 3, skyVS->GetBufferPointer(), skyVS->GetBufferSize(), &skyboxInputLayout);
    m_device->CreateInputLayout(pplayout, 2, ppVS->GetBufferPointer(), ppVS->GetBufferSize(), &ppinputLayout);
    
    ID3D11SamplerState* ImageSamplerState = nullptr;
    D3D11_SAMPLER_DESC ImageSamplerDesc = {};

    ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    ImageSamplerDesc.MipLODBias = 0.0f;
    ImageSamplerDesc.MaxAnisotropy = 1;
    ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
    ImageSamplerDesc.MinLOD = -FLT_MAX;
    ImageSamplerDesc.MaxLOD = FLT_MAX;

    m_device->CreateSamplerState(&ImageSamplerDesc,
        &ImageSamplerState);

    SetD3DStates();

    auto viewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float)width, (float)height,0.0f,1.0f);
    auto shadowViewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float)2048, (float)2048, 0.0f, 1.0f);
    m_deviceContext->RSSetViewports(1, &viewport);

    std::vector<const char*> skyFaces;
    skyFaces.push_back("sky_wasteland02lf.png");
    skyFaces.push_back("sky_wasteland02rt.png");
    skyFaces.push_back("sky_wasteland02up.png");
    skyFaces.push_back("sky_wasteland02dn.png");
    skyFaces.push_back("sky_wasteland02ft.png");
    skyFaces.push_back("sky_wasteland02bk.png");

    Texture* albedo = new Texture("rustdiffuse.jpg", m_device, m_deviceContext);
    Texture* roughness = new Texture("rustspec.jpg", m_device, m_deviceContext);
    Texture* normal = new Texture("TilesNormal.jpg", m_device, m_deviceContext);
    Texture* ao = new Texture("TilesAO.jpg", m_device, m_deviceContext);
    Texture* heightMap = new Texture("Terrain005_4K.png", m_device, m_deviceContext);
    Texture* specular = new Texture("rustspec.jpg", m_device, m_deviceContext);
    Texture* skybox = new Texture(skyFaces, m_device, m_deviceContext);
    //Texture* textures = { {"diffuse.jpg", m_device, m_deviceContext},{"specular.jpg", m_device, m_deviceContext} };
    //textures[0] = diffuse;
    //textures[1] = specular;

    //m_deviceContext->IASetIndexBuffer(lModel.meshes[0].indexBuffer, DXGI_FORMAT_R16_UINT, 0u);
    
    float clearColor[] = { 0.25f,0.5f,1,1 };
    glm::mat4 model = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 prevProjection = glm::mat4(1.0f);
    glm::mat4 prevView = glm::mat4(1.0f);
    glm::mat4 objectMatrix = glm::mat4(1.0f);
    glm::mat4 lightView = glm::mat4(1.0f);
    glm::mat4 lightProjection = glm::mat4(1.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
    float radius = 10.0f;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    //Texture* tex = nullptr;
    //std::vector<Texture*> loaded_tex;
    //diffuse->ImageShaderResourceView
    //m_rtSRV
    m_deviceContext->PSSetShaderResources(0, 1, &albedo->ImageShaderResourceView);
    m_deviceContext->PSSetShaderResources(1, 1, &roughness->ImageShaderResourceView);
    m_deviceContext->PSSetShaderResources(3, 1, &normal->ImageShaderResourceView);
    m_deviceContext->PSSetShaderResources(4, 1, &ao->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(0, 1, &albedo->ImageSamplerState);



    m_deviceContext->PSSetShaderResources(20, 1, &skybox->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(15, 1, &skybox->ImageSamplerState);


    m_deviceContext->PSSetShaderResources(1, 1, &specular->ImageShaderResourceView);
    m_deviceContext->PSSetSamplers(1, 1, &specular->ImageSamplerState);
    float tme = 0;

    AudioDevice device;
    AudioListener listener;
    AudioSource source(device.device, device.context);
    source.listener = listener;
    source.setPosition(10.0f, 2.0f, 5.0f);
    //source.play("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/aby_music_stuntm.mp3");
    source.playM("sewer_air1.wav");
    source.looping = true;

    //Terrain terrain(257, 257, m_device, m_deviceContext);

    while (!finishedRendering)
    {
        ID3D11ShaderResourceView* nullsrv = nullptr;
        m_deviceContext->PSSetShaderResources(18, 1, &nullsrv);
        m_deviceContext->PSSetShaderResources(8, 1, &nullsrv);
        physicsSystem.isSimulating = true;
        SDLInput();
        listener.setPosition(camera.Position);
        listener.setRotation(camera.Front, camera.WorldUp);
        source.listener = listener;

        //Entity* physCubeEnt = nullptr;
        //physCubeEnt = currentActiveScene->GetEntity(&physCube);
        //glm::vec3 physPos = glm::vec3(physicsSystem.GetRigidbody(sphereBody).position.GetX(), physicsSystem.GetRigidbody(sphereBody).position.GetY(), physicsSystem.GetRigidbody(sphereBody).position.GetZ());
        //glm::vec3 physRot = glm::vec3(physicsSystem.GetRigidbody(sphereBody).rotation.GetX(), physicsSystem.GetRigidbody(sphereBody).rotation.GetY(), physicsSystem.GetRigidbody(sphereBody).rotation.GetZ());
        //physCubeEnt->setPosition(physPos);
        //physCubeEnt->setRotation(physRot);

        m_deviceContext->OMSetRenderTargets(1u, &m_renderTargetView, m_depthStencilView);

        m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
        m_deviceContext->RSSetState(m_rasterizerState);
        m_deviceContext->OMSetBlendState(m_blendState, NULL, 0xffffffff);

        // matrix stuff under this
        float camX = std::sin(tme) * radius;
        float camZ = std::cos(tme) * radius;
        //view = glm::lookAtRH(glm::vec3(camX, 1.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //glm::vec3 lightDir = camera.Position + glm::vec3(0.0f, -1.0f, 0.5f);
        view = camera.GetViewMatrix();
        //view = glm::lookAt(glm::vec3(4.0f, -1.0f, 0.5f), lightDir, glm::vec3(0, 1, 0));
        projection = glm::perspectiveRH_ZO(glm::radians(camera.Fov), (float)width / (float)height, 0.01f, 10000.0f);
        //projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 10000.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ConstantBuffer scb = {
        model,
        glm::mat4(glm::mat3(view)),
        projection,
        lightSpaceMatrix,
        //DirectX::XMMATRIX(),
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
        m_deviceContext->VSSetShader(skyboxShader.vertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(skyboxShader.pixelShader, nullptr, 0);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        skyCube.frustumCulling = false;
        skyCube.DrawModel(m_deviceContext, m_device, camera.cameraFrustum, false);

        //m_deviceContext->DSSetShaderResources(0, 1, &heightMap->ImageShaderResourceView);
        //m_deviceContext->DSSetSamplers(0, 1, &heightMap->ImageSamplerState);
        //entity.UpdateMatrixes(view, projection);
        //mask.setPosition(glm::vec3(sphereBody.position.GetX(), sphereBody.position.GetY(), sphereBody.position.GetZ()));
        //mask.setRotation(glm::vec3(sphereBody.rotation.GetX(), sphereBody.rotation.GetY(), sphereBody.rotation.GetZ()));
        Light playerLight;
        m_deviceContext->OMSetRenderTargets(0, nullptr, m_shadowDepthStencilView);
        m_deviceContext->RSSetViewports(1, &shadowViewport);
        m_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        //glm::vec3(4.0f, -1.0f, 0.5f)


        glm::vec3 lightDir = camera.Position + glm::vec3(0.0f, -1.0f, 0.5f);
        glm::vec3 lightDir2 = glm::vec3(0.0f, -1.0f, 0.5f);
        glm::mat4 lightDirMatrix = glm::lookAt(camera.Position, lightDir2, glm::vec3(0, 1, 0));
        lightView = glm::lookAtLH(camera.Position, lightDir, glm::vec3(0, 1, 0));
        glm::vec3 cameraLightVector = camera.Position - glm::vec3(lightDir2.x, lightDir2.y, lightDir2.z);
        /*
        glm::mat4 M = glm::mat4(1.0f);

        float NearZ = 0.01f;
        float FarZ = 200.0f;
        float ViewWidth = 20.0f;
        float ViewHeight = 20.0f;

        float fRange = 1.0f / (FarZ - NearZ);

        M[0][0] = 2.0f / ViewWidth;
        M[0][1] = 0.0f;
        M[0][2] = 0.0f;
        M[0][3] = 0.0f;

        M[1][0] = 0.0f;
        M[1][1] = 2.0f / ViewHeight;
        M[1][2] = 0.0f;
        M[1][3] = 0.0f;

        M[2][0] = 0.0f;
        M[2][1] = 0.0f;
        M[2][2] = fRange;
        M[2][3] = 0.0f;

        M[3][0] = 0.0f;
        M[3][1] = 0.0f;
        M[3][2] = -fRange * NearZ;
        M[3][3] = 1.0f;
        */
        lightProjection = glm::orthoLH(-20.0f, 20.0f, -20.0f, 20.0f, 0.01f, 200.0f);
        //lightProjection = M;

        // scuffed garbage to fix shadows maybe

        DirectX::XMVECTOR camPosXM = DirectX::XMVectorSet(camera.Position.x, camera.Position.y, camera.Position.z, 1.0f);
        DirectX::XMVECTOR lightDirXM = DirectX::XMVectorSet(lightDir.x, lightDir.y, lightDir.z, 1.0f);
        DirectX::XMVECTOR upXM = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
        DirectX::XMMATRIX lightview = DirectX::XMMatrixLookAtLH(camPosXM, lightDirXM, upXM);
        DirectX::XMMATRIX lightproj = DirectX::XMMatrixOrthographicLH(20.0f, 20.0f, 1.0f, 1000.0f);

        //DirectX::XMMATRIX lsm = lightproj * lightview;
        //lightProjection = glm::perspectiveRH_ZO(glm::radians(camera.Zoom), 1.0f, 0.1f, 1000.0f);
        //lightSpaceMatrix = lightView * lightProjection;
        Bistro.useMaterials = false;
        for (int i = 0; i < currentActiveScene->sceneObjects.size(); i++) {
            currentActiveScene->sceneObjects[i]->UpdateMatrixes(lightView, lightProjection, camera.Position);

            SconstantBuffer scb = {
                currentActiveScene->sceneObjects[i]->Matmodel,
                lightView,
                lightProjection
            };

            ID3D11Buffer* constantBuffer;
            D3D11_BUFFER_DESC cbd;
            cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbd.Usage = D3D11_USAGE_DYNAMIC;
            cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            cbd.MiscFlags = 0u;
            cbd.ByteWidth = sizeof(scb);
            cbd.StructureByteStride = 0u;
            D3D11_SUBRESOURCE_DATA csd = {};
            csd.pSysMem = &scb;

            m_device->CreateBuffer(&cbd, &csd, &constantBuffer);
            m_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
            //m_deviceContext->DSSetConstantBuffers(0, 1, &constantBuffer);
            //m_deviceContext->HSSetConstantBuffers(0, 1, &constantBuffer);


            //playerLight.setPosition(glm::vec3(4.0f, -1.0f, 0.5f));
            //playerLight.setRotation(glm::vec3(0.0f, -1.0f, 0.5f));
            //playerLight.setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
            //playerLight.setAmbientColor(glm::vec3(0.1f, 0.1f, 0.1f));
            //playerLight.intensity = 1;
            //playerLight.UpdateLightBuffer(m_device, m_deviceContext);


            //m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
            //D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
            //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
            //m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

            m_deviceContext->IASetInputLayout(inputLayout);
            m_deviceContext->VSSetShader(shadowShader.vertexShader, nullptr, 0);
            //m_deviceContext->HSSetShader(hullShader, nullptr, 0);
            //m_deviceContext->DSSetShader(domainShader, nullptr, 0);
            m_deviceContext->PSSetShader(nullptr, nullptr, 0);
            //Rendering stuff under this
            //currentActiveScene->sceneObjects[i]->model.GetClosestVertex(camera.Position);
            currentActiveScene->sceneObjects[i]->Update();
            currentActiveScene->sceneObjects[i]->DrawModel(m_deviceContext, m_device,camera.cameraFrustum, false);
        }
        Bistro.useMaterials = true;
        m_deviceContext->RSSetViewports(1, &viewport);
        m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
        m_deviceContext->OMSetRenderTargets(1u, &m_renderTargetView, m_depthStencilView);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        view = camera.GetViewMatrix();
        projection = glm::perspectiveRH_ZO(glm::radians(camera.Fov), (float)width / (float)height, 0.01f, 10000.0f);
        camera.createFrustumFromCamera((float)width / (float)height, glm::radians(camera.Fov), 0.01f, 10000.0f);
        //m_deviceContext->CopyResource(m_shadowMap2, m_shadowMap);
        ID3D11SamplerState* ImageSamplerState2 = nullptr;
        D3D11_SAMPLER_DESC ImageSamplerDesc2 = {};
        ImageSamplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        ImageSamplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        ImageSamplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        ImageSamplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        ImageSamplerDesc2.MipLODBias = 0.0f;
        ImageSamplerDesc2.MaxAnisotropy = 1;
        ImageSamplerDesc2.BorderColor[0] = 1.0f;
        ImageSamplerDesc2.BorderColor[1] = 1.0f;
        ImageSamplerDesc2.BorderColor[2] = 1.0f;
        ImageSamplerDesc2.BorderColor[3] = 1.0f;
        ImageSamplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        ImageSamplerDesc2.MinLOD = 0;
        ImageSamplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

        m_device->CreateSamplerState(&ImageSamplerDesc2,
            &ImageSamplerState2);

        m_deviceContext->PSSetShaderResources(18, 1, &m_shadowSRV);
        m_deviceContext->PSSetSamplers(13, 1, &ImageSamplerState2);

        for (int i = 0; i < currentActiveScene->sceneObjects.size(); i++) {
            currentActiveScene->sceneObjects[i]->UpdateMatrixes(view, projection, camera.Position);
            
            //objectMatrix = currentActiveScene->sceneObjects[i]->Matmodel * view * projection;
            //lightSpaceMatrix = lightProjection * lightView;
            ConstantBuffer2 cb = {
                currentActiveScene->sceneObjects[i]->Matmodel,
                view,
                projection,
                //lightSpaceMatrix,
                //lsm,
                lightView,
                lightProjection,
                camera.Position,
                0.0f
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


            playerLight.setPosition(glm::vec3(4.0f, 1.0f, 0.5f));
            playerLight.setRotation(glm::vec3(0.0f, -1.0f, 0.5f));
            playerLight.setLightColor(glm::vec3(1.0f, 1.0f, 1.0f));
            playerLight.setAmbientColor(glm::vec3(0.1f, 0.1f, 0.1f));
            playerLight.intensity = 1;
            playerLight.UpdateLightBuffer(m_device, m_deviceContext);
           

            //m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
            //D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
            //D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
            //m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

            m_deviceContext->IASetInputLayout(inputLayout);
            m_deviceContext->VSSetShader(pbrShader.vertexShader, nullptr, 0);
            //m_deviceContext->HSSetShader(hullShader, nullptr, 0);
            //m_deviceContext->DSSetShader(domainShader, nullptr, 0);
            m_deviceContext->PSSetShader(pbrShader.pixelShader, nullptr, 0);

            //Rendering stuff under this
            //currentActiveScene->sceneObjects[i]->model.GetClosestVertex(camera.Position)
            currentActiveScene->sceneObjects[i]->Update();
            currentActiveScene->sceneObjects[i]->DrawModel(m_deviceContext, m_device,camera.cameraFrustum, true);
        }

        glm::mat4 terrainModel = glm::mat4(0.0f);
        terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, 0.0f, 0.0f));
        terrainModel = glm::rotate(terrainModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        terrainModel = glm::scale(terrainModel, glm::vec3(1.0f, 1.0f, 1.0f));

        Terrain::ConstBuffer constBuf = {
            terrainModel,
            view,
            projection,
            camera.Position,
            0.0f
        };

        //m_deviceContext->IASetInputLayout(inputLayout);
        //terrain.DrawTerrain(constBuf, terrainVertexShader, terrainPixelShader, m_deviceContext, m_device);
        

        m_deviceContext->CopyResource(m_renderTexture, m_renderTexture2);
        //m_deviceContext->CopyResource(m_bloomTexture2, m_renderTexture);
        m_deviceContext->CopyResource(m_depthTexture, m_depthBuffer);
        m_deviceContext->PSSetShaderResources(14, 1, &m_depthSRV);
        
        //m_deviceContext->PSSetShaderResources(15, 1, &blurSRV);
        m_deviceContext->PSSetShaderResources(15, 1, &m_rtSRV);
        m_deviceContext->PSSetSamplers(14, 1, &albedo->ImageSamplerState);

        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        m_deviceContext->OMSetRenderTargets(1u, &m_renderTargetView2, m_depthStencilView);
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,

        -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f
        };
      
        //m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        ID3D11Buffer* vertexBuffer = nullptr;
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(Vertex2) * static_cast<UINT>(6);
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0u;
        D3D11_SUBRESOURCE_DATA vertexData = { 0 };
        vertexData.pSysMem = quadVertices;

        m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

        UINT stride = sizeof(Vertex2);
        UINT offset = 0;

        //m_deviceContext->OMSetRenderTargets(1u, &m_renderTargetView2, m_depthStencilView);
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        PostProcessBuffer pcb = {
            prevView,
            prevProjection,
            view,
            projection,
            cameraLightVector,
           //(glm::inverse(view) * glm::inverse(projection)),
            camera.Position,
            0.0f,
            0.0f
        };
        ID3D11Buffer* pconstantBuffer;
        D3D11_BUFFER_DESC pcbd;
        pcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        pcbd.Usage = D3D11_USAGE_DYNAMIC;
        pcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        pcbd.MiscFlags = 0u;
        pcbd.ByteWidth = sizeof(pcb);
        pcbd.StructureByteStride = 0u;
        D3D11_SUBRESOURCE_DATA pcsd = {};
        pcsd.pSysMem = &pcb;

        m_device->CreateBuffer(&pcbd, &pcsd, &pconstantBuffer);
        m_deviceContext->PSSetConstantBuffers(0, 1, &pconstantBuffer);
        m_deviceContext->PSSetShaderResources(8, 1, &m_shadowSRV);

        m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        //m_deviceContext->IASetIndexBuffer(model.meshes[i].indexBuffer, DXGI_FORMAT_R16_UINT, 0u);
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_deviceContext->IASetInputLayout(ppinputLayout);
        //m_deviceContext->HSSetShader(nullptr, nullptr, 0);
        //m_deviceContext->DSSetShader(nullptr, nullptr, 0);
        m_deviceContext->VSSetShader(postprocessShader.vertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(postprocessShader.pixelShader, nullptr, 0);
        //quad.DrawModel(m_deviceContext, m_device);
        m_deviceContext->Draw(6, 0);
        //prevProjection = (view * projection);
        m_swapChain->Present(1, 0);
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = ((NOW - LAST) / (float)SDL_GetPerformanceFrequency());
        tme += deltaTime;
        std::cout << 1.0f / deltaTime << '\n';
        skyConstantBuffer->Release();
        //vertexBuffer->Release();

        prevProjection = projection;
        prevView = view;

    }
    //vertexBuffer->Release();
    //vertexShader->Release();
    //pixelShader->Release();
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
        true,
        1, 0, 0, false,
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
        window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN);
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

    //m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    m_device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &hullShader);
    m_device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &domainShader);
    //m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);

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
    glm::mat4 objectMatrix = glm::mat4(1.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
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
        projection = glm::perspectiveRH_ZO(glm::radians(camera.Fov), (float)width / (float)height, 0.1f, 10000.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        objectMatrix = model * view * projection;
        
        ConstantBuffer cb = {
            model,
            glm::mat4(glm::mat3(camera.GetViewMatrix())),
            projection,
            lightSpaceMatrix,
            //DirectX::XMMATRIX()
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
        //m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
        //m_deviceContext->HSSetShader(hullShader, nullptr, 0);
        //m_deviceContext->DSSetShader(domainShader, nullptr, 0);
        //m_deviceContext->PSSetShader(pixelShader, nullptr, 0);

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
    //vertexShader->Release();
    //pixelShader->Release();
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