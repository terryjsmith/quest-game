
#include <Render/QGMeshComponent.h>
#include <IO/QGResourceSystem.h>
#include <Core/QGApplication.h>

void QGMeshComponent::Serialize(QGDataRecord* record) {
	if (mesh) {
		record->Set("mesh", mesh->resource->path + "/" + mesh->resource->filename);
	}
}

void QGMeshComponent::Deserialize(QGDataRecord* record) {
	std::string current = (mesh == 0) ? "" : mesh->resource->path + "/" + mesh->resource->filename;
	std::string prospect = record->Get("mesh").AsString();
	if (current != prospect) {
		QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
		mesh = (QGMesh*)resourceSystem->Load(prospect, "Mesh");
	}
}