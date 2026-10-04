
#include <Render/QGMeshComponent.h>
#include <IO/QGResourceSystem.h>
#include <Core/QGApplication.h>

void QGMeshComponent::Serialize(QGDataRecord* record) {
	if (mesh) {
		record->Set("mesh", mesh->resource->path + "/" + mesh->resource->filename);
	}

	record->Set("position", this->transform.position);
	record->Set("scaling", this->transform.scaling);
	record->Set("rotation", this->transform.rotation);
}

void QGMeshComponent::Deserialize(QGDataRecord* record) {
	this->transform.position = record->Get("position").AsVector3();
	this->transform.scaling = record->Get("scaling").AsVector3();
	this->transform.rotation = record->Get("rotation").AsQuaternion();

	std::string current = (mesh == 0) ? "" : mesh->resource->path + "/" + mesh->resource->filename;
	std::string prospect = record->Get("mesh").AsString();
	if (current != prospect) {
		QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
		mesh = (QGMesh*)resourceSystem->Load(prospect, "Mesh");
	}
}