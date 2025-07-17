#include "AssetManager.h"

int assetIndex;

AssetPacker::LoadedFile AssetManager::LoadAsset(const char* name) {
	//Asset loadedAsset;
	//loadThread = std::thread(&AssetManager::LoadAssetFromPak, this, name, std::ref(loadedAsset));
	//std::future<Asset> retAsset = std::async(std::launch::async, AssetManager::LoadAssetFromPak, name);
	//Asset loadedAsset;
	AssetPacker::LoadedFile loadedFile;
	return loadedFile;
}

AssetPacker::LoadedFile AssetManager::LoadAssetFromPak(const char* name) {
	/*
	Asset newAsset;
	
	newAsset.IsLoaded = false;
	newAsset.name = name;
	newAsset.data = nullptr;
	newAsset.size = 0;
	loadedAssets.push_back(newAsset);
	Asset assetRef;
	for (int i = 0; i < loadedAssets.size(); i++) {
		if (loadedAssets[i].name == name) {
			assetRef = loadedAssets[i];
			assetIndex = i;
		}
	}
	*/
	AssetPacker packer;
	AssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", name);
	for (int i = 0; i < loadedAssets.size(); i++) {
		if (loadedAssets[i].fileName == lFile.fileName) {
			free(lFile.data);
			return loadedAssets[i];
		}
	}
	loadedAssets.push_back(lFile);

	return lFile;
}