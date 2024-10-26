#include "Entity.h"

Entity::Entity(const char* Ename, EntityType entType) {
	name = Ename;
	type = entType;
}

Entity::~Entity() {

}

void Entity::setModel(Model Imodel) {
	model = Imodel;
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

void Entity::DrawModel(ID3D11DeviceContext* m_deviceContext, ID3D11Device* m_device) {

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
        if (model.loadMaterials) {
            for (int m = 0; m < matManager.materials.size(); m++) {
                if (model.meshes[i].material == matManager.materials[m].name) {
                    m_deviceContext->PSSetShaderResources(0, 1, &matManager.materials[m].diffuse->ImageShaderResourceView);
                    m_deviceContext->PSSetSamplers(0, 1, &matManager.materials[m].diffuse->ImageSamplerState);
                }
            }
        }
        m_deviceContext->DrawIndexed((UINT)model.meshes[i].indices.size(), 0u, 0u);
    }
}