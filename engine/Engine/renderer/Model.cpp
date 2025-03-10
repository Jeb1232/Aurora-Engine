#include "Model.h"

Model::~Model() {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].vertexBuffer->Release();
        meshes[i].indexBuffer->Release();
    }
}


void Model::LoadModel(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (!scene || !scene->mMeshes) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        Mesh newMesh;
        //std::vector<Texture*> textures;

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        aiString str2;
        material->GetTexture(aiTextureType_SPECULAR, 0, &str2);
        aiString str3;
        material->GetTexture(aiTextureType_NORMALS, 0, &str3);
        //std::string tex = material->GetName().C_Str();
        //std::cout << "mat name: " << str.C_Str() << std::endl;
        //std::cout << "mat name Spec: " << str2.C_Str() << std::endl;
        //std::cout << "mat name Norm: " << str3.C_Str() << std::endl;
        newMesh.material = str.C_Str();
        newMesh.materialSpec = str2.C_Str();
        newMesh.materialname = material->GetName().C_Str();
        newMesh.materialNorm = str3.C_Str();
        //newMesh.diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        //textures.insert(textures.end(), newMesh.diffuseMaps.begin(), newMesh.diffuseMaps.end());
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
        {

            auto&& vertex = mesh->mVertices[j];

            minAABB.x = std::min(minAABB.x, vertex.x);
            minAABB.y = std::min(minAABB.y, vertex.y);
            minAABB.z = std::min(minAABB.z, vertex.z);

            maxAABB.x = std::max(maxAABB.x, vertex.x);
            maxAABB.y = std::max(maxAABB.y, vertex.y);
            maxAABB.z = std::max(maxAABB.z, vertex.z);

        }
        //std::cout << "Min Pos: " << minAABB.x << " " << minAABB.y << " " << minAABB.z << " " << std::endl;
        //std::cout << "Max Pos: " << maxAABB.x << " " << maxAABB.y << " " << maxAABB.z << " " << std::endl;
        newMesh.boundingBox = AABB(minAABB, maxAABB);
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            aiVector3D pos = mesh->mVertices[j];
            
            //aiColor4D color = mesh->HasVertexColors(0) ? mesh->mColors[0][j] : aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);  // Default normal if absent
            aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.0f, 0.0f, 0.0f);
            if (!mesh->HasTangentsAndBitangents()) {
                std::cout << "Mesh does not have tangents or bitangents!" << std::endl;
            }
            aiVector3D tangent = mesh->mTangents[j];
            aiVector3D bitangent = mesh->mBitangents[j];

            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            // 1. diffuse maps
            Vertex vertex = {
                pos.x, pos.y, pos.z,
                normal.x, normal.y, normal.z,
                uv.x, uv.y,
                //tangent.x,tangent.y,tangent.z,
                //bitangent.x,bitangent.y,bitangent.z
            };
            //vertex.pos = glm::vec3(pos.x, pos.y, pos.z);
            //vertex.normals = glm::vec3(normal.x, normal.y, normal.z);
            //vertex.uvs = glm::vec2(uv.x, uv.y);
            newMesh.vertices.push_back(vertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            if (face.mNumIndices == 3) {
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
            }
        }
        meshes.push_back(newMesh);
    }

    for (int i = 0; i < meshes.size(); i++)
    {
        ID3D11Buffer* vertexBuffer = nullptr;
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(meshes[i].vertices.size());
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.StructureByteStride = sizeof(Vertex);
        meshes[i].vertexBuffer = vertexBuffer;
        D3D11_SUBRESOURCE_DATA vertexData = { 0 };
        vertexData.pSysMem = meshes[i].vertices.data();

        ID3D11Buffer* indexBuffer = nullptr;
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags = 0u;
        ibd.MiscFlags = 0u;
        ibd.ByteWidth = sizeof(unsigned short) * meshes[i].indices.size();
        ibd.StructureByteStride = sizeof(unsigned short);
        meshes[i].indexBuffer = indexBuffer;
        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = meshes[i].indices.data();

        meshes[i].vertexBufferDesc = vertexBufferDesc;
        meshes[i].vertexData = vertexData;
        meshes[i].ibd = ibd;
        meshes[i].isd = isd;

        m_device->CreateBuffer(&meshes[i].vertexBufferDesc, &meshes[i].vertexData, &meshes[i].vertexBuffer);
        m_device->CreateBuffer(&meshes[i].ibd, &meshes[i].isd, &meshes[i].indexBuffer);

        delete[] vertexBuffer;
        delete[] indexBuffer;
    }
}

float Distance2(glm::vec3 v1, glm::vec3 v2) {
    glm::vec3 tmpV3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    return std::sqrt(tmpV3.x * tmpV3.x + tmpV3.y * tmpV3.y + tmpV3.z * tmpV3.z);
}

void Model::LoadModelFromPAK(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
    Assimp::Importer importer;
    AssetPacker packer;
    AssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", path);
   // AssetManager aManager;
    //AssetManager::Asset loadedAsset = aManager.LoadAsset(path.c_str());
    const aiScene* scene = importer.ReadFileFromMemory(lFile.data, lFile.fileSize,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals | aiProcess_CalcTangentSpace);
    
    free(lFile.data);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    if (!scene || !scene->mMeshes) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        Mesh newMesh;
        //std::vector<Texture*> textures;

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if (loadMaterials) {
            newMesh.material = material->GetName().C_Str();
        }
        //textures.insert(textures.end(), newMesh.diffuseMaps.begin(), newMesh.diffuseMaps.end());
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) 
        {

                auto&& vertex = mesh->mVertices[j];
                
                    minAABB.x = std::min(minAABB.x, vertex.x);
                    minAABB.y = std::min(minAABB.y, vertex.y);
                    minAABB.z = std::min(minAABB.z, vertex.z);

                    maxAABB.x = std::max(maxAABB.x, vertex.x);
                    maxAABB.y = std::max(maxAABB.y, vertex.y);
                    maxAABB.z = std::max(maxAABB.z, vertex.z);
            
        }
        //std::cout << "Min Pos: " << minAABB.x << " " << minAABB.y << " " << minAABB.z << " " << std::endl;
        //std::cout << "Max Pos: " << maxAABB.x << " " << maxAABB.y << " " << maxAABB.z << " " << std::endl;
        newMesh.boundingBox = AABB(minAABB, maxAABB);
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            aiVector3D pos = mesh->mVertices[j];
            //aiColor4D color = mesh->HasVertexColors(0) ? mesh->mColors[0][j] : aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);  // Default normal if absent
            aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.0f, 0.0f, 0.0f);
            if (!mesh->HasTangentsAndBitangents()) {
                std::cout << "Mesh does not have tangents or bitangents!" << std::endl;
            }
            aiVector3D tangent = mesh->mTangents[j];
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            // 1. diffuse maps
            Vertex vertex = {
                pos.x, pos.y, pos.z,
                normal.x, normal.y, normal.z,
                uv.x, uv.y,
                //tangent.x,tangent.y,tangent.z
            };
            //vertex.pos = glm::vec3(pos.x, pos.y, pos.z);
            //vertex.normals = glm::vec3(normal.x, normal.y, normal.z);
            //vertex.uvs = glm::vec2(uv.x, uv.y);
            newMesh.vertices.push_back(vertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            if (face.mNumIndices == 3) {
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
            }
        }
        meshes.push_back(newMesh);
    }

    for (int i = 0; i < meshes.size(); i++)
    {
        ID3D11Buffer* vertexBuffer = nullptr;
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(meshes[i].vertices.size());
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0u;
        vertexBufferDesc.StructureByteStride = sizeof(Vertex);
        meshes[i].vertexBuffer = vertexBuffer;
        D3D11_SUBRESOURCE_DATA vertexData = { 0 };
        vertexData.pSysMem = meshes[i].vertices.data();

        ID3D11Buffer* indexBuffer = nullptr;
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags = 0u;
        ibd.MiscFlags = 0u;
        ibd.ByteWidth = sizeof(unsigned short) * meshes[i].indices.size();
        ibd.StructureByteStride = sizeof(unsigned short);
        meshes[i].indexBuffer = indexBuffer;
        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = meshes[i].indices.data();

        meshes[i].vertexBufferDesc = vertexBufferDesc;
        meshes[i].vertexData = vertexData;
        meshes[i].ibd = ibd;
        meshes[i].isd = isd;

        std::cout << meshes[i].vertexBufferDesc.BindFlags << std::endl;

        result = m_device->CreateBuffer(&meshes[i].vertexBufferDesc, &meshes[i].vertexData, &meshes[i].vertexBuffer);
        result = m_device->CreateBuffer(&meshes[i].ibd, &meshes[i].isd, &meshes[i].indexBuffer);

        delete[] vertexBuffer;
        delete[] indexBuffer;
    }
    
}
/*
void Model::LoadModelThread(const std::string& path, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {
    Assimp::Importer importer;
    AssetPacker packer;
    AssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", path);
    AssetManager aManager;
    AssetManager::Asset loadedAsset = aManager.LoadAsset(path.c_str());
    while (!loadedAsset.IsLoaded) {

    }
    std::cout << loadedAsset.name << std::endl;
    const aiScene* scene = importer.ReadFileFromMemory(loadedAsset.data, loadedAsset.size,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals);
    free(lFile.data);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    if (!scene || !scene->mMeshes) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        Mesh newMesh;
        //std::vector<Texture*> textures;

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if (loadMaterials) {
            newMesh.material = material->GetName().C_Str();
        }
        //textures.insert(textures.end(), newMesh.diffuseMaps.begin(), newMesh.diffuseMaps.end());

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            aiVector3D pos = mesh->mVertices[j];
            //aiColor4D color = mesh->HasVertexColors(0) ? mesh->mColors[0][j] : aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
            aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[j] : aiVector3D(0.0f, 1.0f, 0.0f);  // Default normal if absent
            aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : aiVector3D(0.0f, 0.0f, 0.0f);

            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            // 1. diffuse maps
            Vertex vertex = {
                pos.x, pos.y, pos.z,
                normal.x, normal.y, normal.z,
                uv.x, uv.y,
            };
            //vertex.pos = glm::vec3(pos.x, pos.y, pos.z);
            //vertex.normals = glm::vec3(normal.x, normal.y, normal.z);
            //vertex.uvs = glm::vec2(uv.x, uv.y);
            newMesh.vertices.push_back(vertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace face = mesh->mFaces[j];
            if (face.mNumIndices == 3) {
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[0]));
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[1]));
                newMesh.indices.push_back(static_cast<unsigned short>(face.mIndices[2]));
            }
        }
        meshes.push_back(newMesh);
    }

    for (int i = 0; i < meshes.size(); i++)
    {
        ID3D11Buffer* vertexBuffer = nullptr;
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(meshes[i].vertices.size());
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0u;
        vertexBufferDesc.StructureByteStride = sizeof(Vertex);
        meshes[i].vertexBuffer = vertexBuffer;
        D3D11_SUBRESOURCE_DATA vertexData = { 0 };
        vertexData.pSysMem = meshes[i].vertices.data();

        ID3D11Buffer* indexBuffer = nullptr;
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags = 0u;
        ibd.MiscFlags = 0u;
        ibd.ByteWidth = sizeof(unsigned short) * meshes[i].indices.size();
        ibd.StructureByteStride = sizeof(unsigned short);
        meshes[i].indexBuffer = indexBuffer;
        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = meshes[i].indices.data();

        meshes[i].vertexBufferDesc = vertexBufferDesc;
        meshes[i].vertexData = vertexData;
        meshes[i].ibd = ibd;
        meshes[i].isd = isd;

        std::cout << meshes[i].vertexBufferDesc.BindFlags << std::endl;

        result = m_device->CreateBuffer(&meshes[i].vertexBufferDesc, &meshes[i].vertexData, &meshes[i].vertexBuffer);
        result = m_device->CreateBuffer(&meshes[i].ibd, &meshes[i].isd, &meshes[i].indexBuffer);

        delete[] vertexBuffer;
        delete[] indexBuffer;
    }
}
*/
MaterialManager::Material Model::setMaterial(aiMaterial* mat, aiTextureType type, std::string typeName, ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext) {

    //std::vector<S_Texture> textures;
    std::string filePath;
    //aiGetMaterialString(mat, AI_MATKEY_TEXTURE(type, 0), &path);
    /*
    aiString str;
    aiReturn aRet = mat->GetTexture(type, 0, &str);
    if (aRet == aiReturn_FAILURE) {
        //std::cout << "Assimp failed to get the texture from this mesh" << std::endl;
    }
    */
    std::string fileName = std::string(mat->GetName().C_Str());
    
    std::cout << "Diffuse File Name: " + fileName << std::endl;
    //fileName = directory + "/" + fileName;
    filePath = fileName;
    //if(filePath != "")
    //texture = new Texture(fileName.c_str(), m_device, m_deviceContext);
    for (int i = 0; i < matManager.materials.size(); i++) {
        if (matManager.materials[i].name == fileName.c_str()) {
            MaterialManager::Material tmpMat;
            tmpMat.name = matManager.materials[i].name;
            return tmpMat;
        }
    }
}
