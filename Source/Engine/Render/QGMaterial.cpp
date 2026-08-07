
#include <Render/QGMaterial.h>

QGMaterial::QGMaterial() {
	diffuseColor = vector3(1.0f);
	specularColor = vector3(0.0f);
	emissiveColor = vector3(0.0f);
	metalness = 0.0f;
	roughness = 0.0f;

	diffuseTexture = 0;
	normalTexture = 0;
	aoTexture = 0;
	specularTexture = 0;
	emissiveTexture = 0;
	metalnessTexture = 0;
	roughnessTexture = 0;
}