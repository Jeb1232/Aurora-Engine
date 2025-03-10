#include "Light.h"

Light::Light() {
    intensity = 1.0f;
    lightColor = glm::vec4(1.0f, 1.0f, 1.0f,1.0f);
    ambientColor = glm::vec4(0.1f, 0.1f, 0.1f,1.0f);
}

Light::~Light() {

}

void Light::setPosition(glm::vec3 pos) {
    position = glm::vec4(pos, 1.0f);
}

void Light::setRotation(glm::vec3 rot) {
    direction = glm::vec4(rot, 1.0f);
}

void Light::setLightColor(glm::vec3 color) {
    lightColor = glm::vec4(color,1.0f);
}

void Light::setAmbientColor(glm::vec3 color) {
    ambientColor = glm::vec4(color, 1.0f);
}

void Light::setLightType(LightType type) {
    lightType = type;
}

void Light::UpdateLightBuffer(ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
    LightCB lightBuffer{
        position,
        direction,
        lightColor,
        ambientColor,
        glm::vec3(0.0f,0.0f,0.0f),
        intensity,
    };

    ID3D11Buffer* constantBufferL;
    D3D11_BUFFER_DESC cbdl;
    cbdl.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbdl.Usage = D3D11_USAGE_DYNAMIC;
    cbdl.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbdl.MiscFlags = 0u;
    cbdl.ByteWidth = sizeof(lightBuffer);
    cbdl.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA lcsd = {};
    lcsd.pSysMem = &lightBuffer;
    if (FAILED(m_device->CreateBuffer(&cbdl, &lcsd, &constantBufferL))) {
        std::cout << "Failed to create light constant buffer!" << std::endl;
    }
    m_deviceContext->PSSetConstantBuffers(0, 1, &constantBufferL);
}