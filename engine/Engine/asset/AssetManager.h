#pragma once
#include"../dllHeader.h"
#include"../file/asset_packer.h"
#include<thread>
#include<future>
class AssetManager
{
public:

	struct Asset {
		const char* name;
		int size;
		unsigned char* data;
		bool IsLoaded = false;
	};

	struct AssetPackage {
		const char* name;
		int size;
	};

	Asset AURORAENGINE_API LoadAsset(const char* name);
private:
	std::thread loadThread;
	std::thread assetThread;
	std::vector<Asset> loadedAssets;
	Asset LoadAssetFromPak(const char* name);
	std::vector<AssetPackage> paks;
};

