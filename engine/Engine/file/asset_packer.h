#pragma once
#include"../dllHeader.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<filesystem>
#include<vector>

class AssetPacker {
public:
	struct FileHeader {
		char fileSignature[6];
		float fileVersion;
		int  numOfEntries;
		int encryptValue;
	};
	struct FileTableEntry {
		char fileName[30];
		int fileSize = 0;
		int offset = 0;
	};
	struct LoadedFile {
		std::string fileName;
		int fileSize = 0;
		unsigned char* data = nullptr;
	};
	void AURORAENGINE_API CreatePackage(std::ofstream& pakFile, const std::string& basePath);
	void AURORAENGINE_API ReadPackage(const std::string& pakFilePath, const std::string& outputFolderPath);
	void AURORAENGINE_API LoadPackage(const std::string& pakFilePath);
	LoadedFile AURORAENGINE_API LoadFileFromPackage(const std::string& pakFilePath, const std::string fileName);
	FileHeader m_Header;
	std::vector<LoadedFile> loadedFiles;
private:
};