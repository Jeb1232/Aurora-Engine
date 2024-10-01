#include "Model.h"

Model::~Model() {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i].vertexBuffer->Release();
        meshes[i].indexBuffer->Release();
    }
}

void Model::LoadModel(const std::string& path, ID3D11Device* m_device) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals);

    if (!scene || !scene->mMeshes) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        Mesh newMesh;
        std::vector<S_Texture> textures;

        //aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        //newMesh.diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
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

void Model::LoadModelFromPAK(const std::string& path, ID3D11Device* m_device) {
    Assimp::Importer importer;
    AssetPacker packer;
    AssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", path);
    const aiScene* scene = importer.ReadFileFromMemory(lFile.data, lFile.fileSize,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals);

    free(lFile.data);
    if (!scene || !scene->mMeshes) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        Mesh newMesh;
        std::vector<S_Texture> textures;

        //aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        //newMesh.diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
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
/*
std::vector<Model::S_Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<S_Texture> textures;
    std::string filePath;
    //std::cout << mat->GetTextureCount(type) << std::endl;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        
        mat->GetTexture(type, i, &str);

        std::string fileName = std::string(str.C_Str());
        fileName = directory + "/" + fileName;
        filePath = fileName;

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path, filePath.c_str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            S_Texture texture;
            //std::cout << fileName.c_str() << std::endl;
            texture.path = fileName.c_str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}
*/