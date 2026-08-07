
#ifndef qgcameracomponent_h
#define qgcameracomponent_h

#include <Core/QGComponent.h>
#include <Core/QGTransform.h>

class QUEST_API QGCameraComponent : public QGComponent {
public:
	QGCameraComponent() : fov(60.0f), aspectRatio(1.33333f), fnear(1.0f), ffar(100.0f) {}
	~QGCameraComponent() = default;

	matrix4 ProjectionMatrix();
	matrix4 ViewMatrix();

public:
	void Serialize(QGDataRecord* record);
	void Deserialize(QGDataRecord* record);

public:
	QGTransform transform;

	float fov;
	float aspectRatio;
	float fnear;
	float ffar;
};

#endif