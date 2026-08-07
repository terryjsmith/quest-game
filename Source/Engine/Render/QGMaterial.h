
#ifndef qgmaterial_h
#define qgmaterial_h

#include <Render/QGTexture2D.h>

class QUEST_API QGMaterial : public QGObject {
public:
	QGMaterial();
	~QGMaterial() = default;

public:
	std::string name;

	vector3 diffuseColor;
	vector3 specularColor;
	vector3 emissiveColor;
	float metalness;
	float roughness;

	QGTexture2D* diffuseTexture;
	QGTexture2D* normalTexture;
	QGTexture2D* aoTexture;
	QGTexture2D* specularTexture;
	QGTexture2D* emissiveTexture;
	QGTexture2D* metalnessTexture;
	QGTexture2D* roughnessTexture;
};

#endif