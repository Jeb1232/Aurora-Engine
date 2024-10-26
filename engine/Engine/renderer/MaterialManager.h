#pragma once
#include"Texture.h"
class MaterialManager
{
public:
	struct Material {
		std::string name;
		Texture* diffuse;
	};
	void AddMaterial(Material mat);
	std::vector<Material> materials;
private:
};

