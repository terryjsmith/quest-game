
#include <Render/QGAnimationLoader.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

QGResourceObject* QGAnimationLoader::LoadResource(QGResource* resource, std::string type) {
	// Import the scene
	const struct aiScene* scene = aiImportFileFromMemory((const char*)resource->Data(), resource->filesize, aiProcess_Triangulate, resource->extension.c_str());
	assert(scene != 0);

	// Load animations
	if (scene->HasAnimations()) {
		for (int i = 0; i < scene->mNumAnimations; i++) {
			QGAnimation* animation = new QGAnimation();
			animation->name = scene->mAnimations[i]->mName.C_Str();
			animation->duration = scene->mAnimations[i]->mDuration;
			animation->speed = scene->mAnimations[i]->mTicksPerSecond;

			// For each channel (bone) in the animation, get the keyframes
			for (int c = 0; c < scene->mAnimations[i]->mNumChannels; c++) {
				aiNodeAnim* nodeAnim = scene->mAnimations[i]->mChannels[c];
				std::string nodeName = nodeAnim->mNodeName.C_Str();

				QGAnimationTransforms* transforms = new QGAnimationTransforms();
				for (int s = 0; s < nodeAnim->mNumScalingKeys; s++) {
					QGAnimationTransforms::ScalingKey scaling;
					scaling.time = nodeAnim->mScalingKeys[s].mTime;
					aiVector3D vec = nodeAnim->mScalingKeys[s].mValue;
					scaling.scaling = vector3(vec.x, vec.y, vec.z);

					transforms->scalingKeys[scaling.time] = scaling;
				}

				for (int s = 0; s < nodeAnim->mNumPositionKeys; s++) {
					QGAnimationTransforms::TranslationKey translate;
					translate.time = nodeAnim->mPositionKeys[s].mTime;
					aiVector3D vec = nodeAnim->mPositionKeys[s].mValue;
					translate.position = vector3(vec.x, vec.y, vec.z);

					transforms->translationKeys[translate.time] = translate;
				}

				for (int s = 0; s < nodeAnim->mNumRotationKeys; s++) {
					QGAnimationTransforms::RotationKey rotation;
					rotation.time = nodeAnim->mRotationKeys[s].mTime;
					aiQuaternion quat = nodeAnim->mRotationKeys[s].mValue;
					rotation.rotation = quaternion(quat.w, quat.x, quat.y, quat.z);

					transforms->rotationKeys[rotation.time] = rotation;
				}

				animation->transforms[nodeName] = transforms;
			}

			return(animation);
		}
	}

	return(0);
}
