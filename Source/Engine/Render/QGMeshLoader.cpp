
#include <Render/QGMeshLoader.h>
#include <Render/QGRenderSystem.h>
#include <Core/QGApplication.h>
#include <IO/QGResourceSystem.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

static inline glm::mat4 mat4_convert(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }

QGTexture2D* LoadTexture(aiTextureType type, const aiMaterial* pMaterial, std::string currentPath) {
	aiString Path;
	if (pMaterial->GetTexture(type, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
		// Is this an embedded texture or an actual path to a file?
		std::string firstChar = std::string(Path.C_Str()).substr(0, 1);
		if (firstChar == "*") {
			// Embedded texture, skip?
			QGASSERT(false, "Embedded texture, please help me!");
			return(0);
		}
		else {
			// Otherwise, it is a path, record the texture file
			std::string fileStr = Path.C_Str();
			if (fileStr.find_last_of("/\\") != std::string::npos) {
				fileStr = fileStr.substr(fileStr.find_last_of("/\\") + 1);
			}

			// Load texture file
			QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
			std::string fullPath = currentPath + "/" + fileStr;
			QGTexture2D* texture = (QGTexture2D*)resourceSystem->Load(fullPath, "Texture2D");
			assert(texture != 0);

			return(texture);
		}
	}

	return(0);
}

QGMesh* ProcessMesh(aiMesh* paiMesh, std::vector<QGMaterial*>& materials, matrix4 parentTransform, std::string filetype) {
	QGRenderSystem* renderSystem = GetQGSystem<QGRenderSystem>();

	// Create a mesh
	QGMesh* mesh = new QGMesh();
	mesh->name = paiMesh->mName.C_Str();

	// Save transform
	vector3 skew;
	vector4 perspective;
	glm::decompose(parentTransform, mesh->transform.scaling, mesh->transform.rotation, mesh->transform.position, skew, perspective);

	// If FBX, change scale to meters from centimeters
	if (filetype == "fbx") {
		mesh->transform.scaling /= 100.0f;
	}

	// Assign material
	mesh->diffuseTexture = materials[paiMesh->mMaterialIndex]->diffuseTexture;

	// Create a vertex layout spec
	QGVertexAttributeList* vertexAttributeList = renderSystem->CreateVertexAttributeList();

	int vertexSize = 3;
	vertexAttributeList->AddVertexAttribute(QGVertexAttribute::ATTRIB_POSITION, 0, 3);

	if (paiMesh->HasNormals()) {
		vertexAttributeList->AddVertexAttribute(QGVertexAttribute::ATTRIB_NORMAL, vertexSize, 3);
		vertexSize += 3;
	}

	if (paiMesh->HasVertexColors(0)) {
		vertexAttributeList->AddVertexAttribute(QGVertexAttribute::ATTRIB_COLOR, vertexSize, 3);
		vertexSize += 3;
	}

	if (paiMesh->HasTextureCoords(0)) {
		vertexAttributeList->AddVertexAttribute(QGVertexAttribute::ATTRIB_TEXCOORD0, vertexSize, 2);
		vertexSize += 2;
	}

	if (paiMesh->HasBones()) {
		vertexAttributeList->AddVertexAttribute(QGVertexAttribute::ATTRIB_BONES, vertexSize, 4);
		vertexSize += 4;

		vertexAttributeList->AddVertexAttribute(QGVertexAttribute::ATTRIB_BONEWEIGHTS, vertexSize, 4);
		vertexSize += 4;
	}

	// Temporary buffer
	std::vector<float> vertex_data;
	vertex_data.resize(vertexSize * paiMesh->mNumVertices);

	// Load vertices
	for (int i = 0; i < paiMesh->mNumVertices; i++) {
		int offset = i * vertexSize;

		// Get our vertex data in assimp's format
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : NULL;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : NULL;

		// Save position
		vector4 position = vector4(pPos->x, pPos->y, pPos->z, 1.0);

		vertex_data[offset + 0] = position.x;
		vertex_data[offset + 1] = position.y;
		vertex_data[offset + 2] = position.z;
		offset += 3;

		if (paiMesh->HasNormals()) {
			vector4 normal = vector4(pNormal->x, pNormal->z, pNormal->y, 1.0);
			vertex_data[offset + 0] = normal.x;
			vertex_data[offset + 1] = normal.y;
			vertex_data[offset + 2] = normal.z;

			offset += 3;
		}

		if (paiMesh->HasTextureCoords(0)) {
			vertex_data[offset + 0] = pTexCoord->x;
			vertex_data[offset + 1] = pTexCoord->y;

			offset += 2;
		}
	}

	// Load index data
	std::vector<unsigned int> index_data;
	index_data.resize(paiMesh->mNumFaces * 3);

	for (int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& face = paiMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		int offset = i * 3;

		index_data[offset + 0] = face.mIndices[0];
		index_data[offset + 1] = face.mIndices[1];
		index_data[offset + 2] = face.mIndices[2];
	}

	// Create vertex buffer to load vertex data
	mesh->vertexBuffer = renderSystem->CreateVertexBuffer();
	mesh->vertexBuffer->Create(vertexAttributeList, paiMesh->mNumVertices, vertex_data.data(), false);

	mesh->indexBuffer = renderSystem->CreateIndexBuffer();
	mesh->indexBuffer->Create(index_data.size(), index_data.data());

	return(mesh);
}

std::vector<QGMesh*> ProcessNode(const aiScene* scene, aiNode* node, std::vector<QGMaterial*>& materials, matrix4 parentTransform, std::string filetype) {
	// Calculate cumulative transform
	matrix4 transform = parentTransform * mat4_convert(node->mTransformation);

	// Process any mesh nodes as children of this node
	std::vector<QGMesh*> meshes;
	for (int i = 0; i < node->mNumMeshes; i++) {
		QGMesh* mesh = ProcessMesh(scene->mMeshes[node->mMeshes[i]], materials, transform, filetype);
		meshes.push_back(mesh);
	}

	// Process children
	for (int i = 0; i < node->mNumChildren; i++) {
		std::vector<QGMesh*> newMeshes = ProcessNode(scene, node->mChildren[i], materials, transform, filetype);
		meshes.insert(meshes.end(), newMeshes.begin(), newMeshes.end());
	}

	// Return meshes for root node
	return(meshes);
}

QGResourceObject* QGMeshLoader::LoadResource(QGResource* resource, std::string type) {
	// Create a new mesh
	QGMesh* mesh = new QGMesh();
	std::vector<QGMaterial*> materials;

	// Import the scene
	const struct aiScene* scene = aiImportFileFromMemory((const char*)resource->Data(), resource->filesize, aiProcess_Triangulate, resource->extension.c_str());
	assert(scene != 0);

	// Process materials
	for (int i = 0; i < scene->mNumMaterials; i++) {
		// Create a new material
		QGMaterial* material = new QGMaterial();
		materials.push_back(material);

		const aiMaterial* pMaterial = scene->mMaterials[i];
		material->name = pMaterial->GetName().C_Str();

		// Diffuse
		aiColor3D diffuse(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->diffuseColor = vector3(diffuse.r, diffuse.g, diffuse.b);

		aiColor3D specular(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->specularColor = vector3(specular.r, specular.g, specular.b);

		float metalness = 0.0f;
		pMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metalness);
		material->metalness = metalness;

		float roughness = 0.0f;
		pMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
		material->roughness = roughness;

		// Process textures, start with current path
		std::filesystem::path p(resource->path);
		std::string currentPath = p.string();

		// Could be in PBR version
		material->diffuseTexture = LoadTexture(aiTextureType_BASE_COLOR, pMaterial, currentPath);
		material->normalTexture = LoadTexture(aiTextureType_NORMAL_CAMERA, pMaterial, currentPath);
		material->specularTexture = LoadTexture(aiTextureType_SPECULAR, pMaterial, currentPath);

		// Fallback to legacy locations
		if (material->diffuseTexture == 0) material->diffuseTexture = LoadTexture(aiTextureType_DIFFUSE, pMaterial, currentPath);
		if (material->normalTexture == 0) material->normalTexture = LoadTexture(aiTextureType_NORMALS, pMaterial, currentPath);

		material->emissiveTexture = LoadTexture(aiTextureType_EMISSION_COLOR, pMaterial, currentPath);
		material->metalnessTexture = LoadTexture(aiTextureType_METALNESS, pMaterial, currentPath);
		material->roughnessTexture = LoadTexture(aiTextureType_DIFFUSE_ROUGHNESS, pMaterial, currentPath);
		material->aoTexture = LoadTexture(aiTextureType_AMBIENT_OCCLUSION, pMaterial, currentPath);
	}

	mesh->children = ProcessNode(scene, scene->mRootNode, materials, matrix4(1.0f), resource->extension);

	return(mesh);
}
