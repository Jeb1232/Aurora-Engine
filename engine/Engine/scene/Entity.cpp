#include "Entity.h"
#include"stb_image.h"
#include"DDSTextureLoader.h"

Entity::Entity(const char* Ename, EntityType entType) {
	name = Ename;
	type = entType;
}

Entity::~Entity() {

}

void Entity::setModel(Model Imodel) {
	//model = Imodel;
}

void Entity::setPosition(glm::vec3 pos) {
    if (pos != transform.position) {
        transform.position = pos;
        Matmodel = glm::translate(Matmodel, transform.position);
    }
}

void Entity::setRotation(glm::vec3 rot) {
    if (rot != transform.rotation) {
        transform.rotation = rot;
        Matmodel = glm::rotate(Matmodel, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        Matmodel = glm::rotate(Matmodel, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        Matmodel = glm::rotate(Matmodel, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void Entity::setScale(glm::vec3 scale) {
    if (scale != transform.scale) {
        transform.scale = scale;
        Matmodel = glm::scale(Matmodel, transform.scale);
    }
}

void Entity::Update() {
	
}

void Entity::UpdateMatrixes(glm::mat4 Iview, glm::mat4 Iprojection, glm::vec3 camPos) {
	Matview = Iview;
	Matprojection = Iprojection;
    camPosition = camPos;
}

float GetDistance(float x1, float x2, float y1, float y2, float z1, float z2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float dz = z1 - z2;
    return dx * dx + dy * dy + dz * dz;
}

void Entity::DrawModel(ID3D11DeviceContext* m_deviceContext, ID3D11Device* m_device, Frustum cameraFrustum, bool frustumCull) {

    for (int i = 0; i < model.meshes.size(); i++)
    {
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        if (&model.meshes[0].vertexBuffer == NULL) {
            std::cerr << "vertex buff is broken" << std::endl;
        }

        if (&model.meshes[i].indexBuffer == NULL) {
            std::cerr << "index buff is broken" << std::endl;
        }
        m_deviceContext->IASetVertexBuffers(0, 1, &model.meshes[i].vertexBuffer, &stride, &offset);
        m_deviceContext->IASetIndexBuffer(model.meshes[i].indexBuffer, DXGI_FORMAT_R16_UINT, 0u);
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        if (useMaterials) {
            D3D11_TEXTURE2D_DESC ImageTextureDesc = {};
            D3D11_SUBRESOURCE_DATA ImageSubresourceData = {};
            ID3D11Texture2D* ImageTexture = nullptr;
            ID3D11Texture2D* ImageTextureNorm = nullptr;
            ID3D11Resource* ImageTexture2 = nullptr;
            //ID3D11ShaderResourceView* ImageShaderResourceView = nullptr;
            //ID3D11ShaderResourceView* ImageShaderResourceView2 = nullptr;

            int ImageWidth;
            int ImageHeight;
            int ImageChannels;
            int ImageDesiredChannels = 4;
            int ImageWidth2;
            int ImageHeight2;
            int ImageChannels2;
            int ImageDesiredChannels2 = 4;
            //stbi_set_flip_vertically_on_load(true);

            //AssetPacker packer;
            //ssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", path);
            //std::cout << "debug mat diff: " << model.meshes[i].materialname << std::endl;
            //model.meshes[i].material.substr(9, model.meshes[i].material.length() - 9);
            std::string texName = model.meshes[i].material.substr(9, model.meshes[i].material.length() - 9);
            if (model.meshes[i].material.find("/") != std::string::npos) {
                //texName = model.meshes[i].material.substr(9, model.meshes[i].material.length() - 9);
            }
            //"C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/sponza/textures/"
            //std::cout << texName << std::endl;
            std::string texPath = "C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/sponza/textures/" + texName;
            //model.meshes[i].materialNorm.substr(9, model.meshes[i].materialNorm.length() - 9);
            std::string texNameN = model.meshes[i].materialNorm.substr(9, model.meshes[i].materialNorm.length() - 9);
            //std::cout << texNameN << std::endl;
            if (model.meshes[i].materialNorm.find("/") != std::string::npos) {
                //texNameN = model.meshes[i].materialNorm.substr(9, model.meshes[i].materialNorm.length() - 9);
            }
            std::string texPathN = "C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/sponza/textures/" + texNameN;
            //std::cout << texPath << std::endl;
            //Material newMat;
            //newMat.name = model.meshes[i].materialname;
            
            std::wstring widestr = std::wstring(texPath.begin(), texPath.end());
            const wchar_t* widecstr = widestr.c_str();
            std::string texName2 = "";
            if (model.meshes[i].materialSpec != "") {
                //texName2 = model.meshes[i].materialSpec.substr(9, model.meshes[i].materialSpec.length() - 9);
            }
            std::string texPath2 = "C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/Textures/" + texName2;
            std::wstring widestr2 = std::wstring(texPath2.begin(), texPath2.end());
            const wchar_t* widecstr2 = widestr2.c_str();
            Material newMat;
            newMat.name = model.meshes[i].materialname;
            if (!texturesLoaded) {
                //std::cout << "mat name: " << model.meshes[i].materialname << " " << "texture path: " << texPath << std::endl;
                unsigned char* ImageData = stbi_load(texPath.c_str(),
                    &ImageWidth,
                    &ImageHeight,
                    &ImageChannels, ImageDesiredChannels);

                int ImagePitch = ImageWidth * 4;

                if (!ImageData) {
                    std::cout << "Failed to load image: " << texPath.c_str() << '\n';
                    stbi_image_free(ImageData);
                }


                //std::cout << ImageWidth + " " + ImageHeight << std::endl;
                //free(lFile.data);

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
                    &newMat.diffuse
                );
                m_deviceContext->GenerateMips(newMat.diffuse);
                stbi_image_free(ImageData);
                //DirectX::CreateDDSTextureFromFile(m_device, widecstr, nullptr, &newMat.diffuse);

                if (texNameN != "") {
                    unsigned char* ImageData2 = stbi_load(texPathN.c_str(),
                        &ImageWidth2,
                        &ImageHeight2,
                        &ImageChannels2, ImageDesiredChannels2);

                    int ImagePitch2 = ImageWidth2 * 4;

                    if (!ImageData2) {
                        std::cout << "Failed to load image: " << texPathN.c_str() << '\n';
                        stbi_image_free(ImageData2);
                    }


                    //std::cout << ImageWidth + " " + ImageHeight << std::endl;
                    //free(lFile.data);

                    // Texture

                    ImageTextureDesc.Width = ImageWidth2;
                    ImageTextureDesc.Height = ImageHeight2;
                    ImageTextureDesc.MipLevels = 6;
                    ImageTextureDesc.ArraySize = 1;
                    ImageTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                    ImageTextureDesc.SampleDesc.Count = 1;
                    ImageTextureDesc.SampleDesc.Quality = 0;
                    ImageTextureDesc.Usage = D3D11_USAGE_DEFAULT;
                    ImageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                    ImageTextureDesc.CPUAccessFlags = 0;
                    ImageTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

                    ImageSubresourceData.pSysMem = ImageData2;
                    ImageSubresourceData.SysMemPitch = sizeof(unsigned char) * ImagePitch2;

                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

                    srvDesc.Format = ImageTextureDesc.Format;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MostDetailedMip = 0;
                    srvDesc.Texture2D.MipLevels = 6;

                    m_device->CreateTexture2D(&ImageTextureDesc,
                        nullptr,
                        &ImageTextureNorm
                    );

                    // Sampler

                    m_deviceContext->UpdateSubresource(ImageTextureNorm, 0, nullptr, ImageData2, ImageSubresourceData.SysMemPitch, 0);

                    m_device->CreateShaderResourceView(ImageTextureNorm,
                        &srvDesc,
                        &newMat.normal
                    );
                    m_deviceContext->GenerateMips(newMat.normal);
                    stbi_image_free(ImageData2);
                }

                if (texName2 != "") {
                    //DirectX::CreateDDSTextureFromFile(m_device, widecstr2, nullptr, &newMat.specular);
                }
            }
            if (!texturesLoaded) {
                bool matExists = false;
                for (int m = 0; m < materials.size(); m++) {
                    if (materials[m].name == newMat.name) {
                        matExists = true;
                        break;
                    }
                }
                if (!matExists) {
                    materials.push_back(newMat);
                }
            }
            
            //textures.push_back(ImageShaderResourceView);
            //texturesSpec.push_back(ImageShaderResourceView2);
            
            

                
                
            if (!texturesLoaded) {
                m_deviceContext->PSSetShaderResources(0, 1, &newMat.diffuse);
                if (texNameN != "") {
                    m_deviceContext->PSSetShaderResources(3, 1, &newMat.normal);
                }
                if (texName2 != "") {
                    //m_deviceContext->PSSetShaderResources(1, 1, &newMat.specular);
                }
            }
            else if (texturesLoaded) {
                for (int m = 0; m < materials.size(); m++) {
                    if (materials[m].name == newMat.name) {
                        m_deviceContext->PSSetShaderResources(0, 1, &materials[m].diffuse);
                        m_deviceContext->PSSetShaderResources(3, 1, &materials[m].normal);
                    }
                }
                //m_deviceContext->PSSetShaderResources(0, 1, &materials[i].diffuse);
                if (texName2 != "") {
                    //m_deviceContext->PSSetShaderResources(1, 1, &materials[i].specular);
                }
            }
                //m_deviceContext->PSSetSamplers(0, 1, &matManager.materials[m].diffuse->ImageSamplerState);
        }
        if (model.meshes[i].boundingBox.isOnFrustum(cameraFrustum, Matmodel) && frustumCull) {
            m_deviceContext->DrawIndexed((UINT)model.meshes[i].indices.size(), 0u, 0u);
        }
        else if (!frustumCull) {
            m_deviceContext->DrawIndexed((UINT)model.meshes[i].indices.size(), 0u, 0u);
        }
    }
    if (useMaterials) {
        texturesLoaded = true;
    }
}