
#include <Render/QGAnimationSystem.h>
#include <Core/QGWorld.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGApplication.h>
#include <Render/QGMeshComponent.h>

void QGAnimationSystem::Update(float delta) {
	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	uint64_t currentTick = timeSystem->Tick();

	QGWorld* world = QGWorld::GetInstance();
	std::vector<QGAnimatedMeshComponent*> controllers = world->GetComponents<QGAnimatedMeshComponent>();

	// Update them
	for (auto it = controllers.begin(); it != controllers.end(); it++) {
		QGMesh* mesh = (*it)->mesh;

		// Reset bone matrices
		for (auto bit = mesh->bones.begin(); bit != mesh->bones.end(); bit++) {
			(*it)->boneMatrix[bit->first] = matrix4(1.0f);
		}

		// Only update active animations
		if ((*it)->activeAnimation == 0) {
			this->ProcessNodeHierarchy(mesh->nodes, *it, -1);
			continue;
		}

		// Get active animation and mesh
		QGAnimation* animation = (*it)->activeAnimation;

		// Compute how far we are into the animation playback
		double currentTime = ((float)(currentTick - (*it)->startTime) / QG_TICKS_PER_SECOND);

		// Convert to current frame
		int currentFrame = ((float)currentTime * animation->speed);

		// If we are past the end, and not looping, then stop
		if (currentFrame > animation->duration) {
			if ((*it)->looping == false) {
				(*it)->Stop();
				continue;
			}
			
			// Adjust time back to beginning
			currentFrame = std::fmod(currentFrame, animation->duration);
		}

		this->ProcessNodeHierarchy(mesh->nodes, *it, currentFrame);
	}
}

void QGAnimationSystem::ProcessNodeHierarchy(QGNode3D* node, QGAnimatedMeshComponent* mc, int currentFrame, matrix4 parent) {
	// Use the node matrix by default
	matrix4 localMatrix = node->transform;
	
	// Is this animation actively playing?
	if (currentFrame > 0) {
		// Does this node have a transform in this animation?
		auto bit = mc->activeAnimation->transforms.find(node->name);
		if (bit != mc->activeAnimation->transforms.end()) {
			QGAnimationTransforms* transforms = bit->second;

			// Find the relevant keys
			vector3 scaling;
			if (transforms->scalingKeys.size() > 1) {
				auto sit = transforms->scalingKeys.begin();
				QGAnimationTransforms::ScalingKey* scalingKey1 = (*sit);
				QGAnimationTransforms::ScalingKey* scalingKey2 = (*sit);
				for (; sit != transforms->scalingKeys.end(); sit++) {
					scalingKey2 = (*sit);
					if (scalingKey2->time >= currentFrame) break;
					scalingKey1 = (*sit);
				}

				// Compute difference
				float difference = ((float)currentFrame - scalingKey1->time) / ((float)scalingKey2->time - scalingKey1->time);
				QGASSERT(difference >= 0.0f && difference <= 1.0f, "Error");

				// Interpolate
				vector3 scalediff = scalingKey2->scaling - scalingKey1->scaling;
				scaling = scalingKey1->scaling + (scalediff * difference);
			}
			else {
				scaling = (*transforms->scalingKeys.begin())->scaling;
			}

			// Next up: position
			vector3 position;
			if (transforms->translationKeys.size() > 1) {
				auto tit = transforms->translationKeys.begin();
				QGAnimationTransforms::TranslationKey* positionKey1 = (*tit);
				QGAnimationTransforms::TranslationKey* positionKey2 = (*tit);
				
				for (; tit != transforms->translationKeys.end(); tit++) {
					positionKey2 = (*tit);
					if (positionKey2->time >= currentFrame) break;
					positionKey1 = (*tit);
				}

				// Compute difference
				float difference = ((float)currentFrame - positionKey1->time) / ((float)positionKey2->time - positionKey1->time);
				QGASSERT(difference >= 0.0f && difference <= 1.0f, "Error");

				// Interpolate
				vector3 posdiff = positionKey2->position - positionKey1->position;
				position = positionKey1->position + (posdiff * difference);
			}
			else {
				position = (*transforms->translationKeys.begin())->position;
			}

			// Finally, rotation
			quaternion rotation;
			if (transforms->rotationKeys.size() > 1) {
				auto rit = transforms->rotationKeys.begin();
				QGAnimationTransforms::RotationKey* rotationKey1 = (*rit);
				QGAnimationTransforms::RotationKey* rotationKey2 = (*rit);
				for (; rit != transforms->rotationKeys.end(); rit++) {
					rotationKey2 = (*rit);
					if (rotationKey2->time >= currentFrame) break;
					rotationKey1 = (*rit);
				}

				// Compute difference
				float difference = ((float)currentFrame - rotationKey1->time) / ((float)rotationKey2->time - rotationKey1->time);
				QGASSERT(difference >= 0.0f && difference <= 1.0f, "Error");

				// Interpolate
				rotation = glm::slerp(rotationKey1->rotation, rotationKey2->rotation, difference);
			}
			else {
				rotation = (*transforms->rotationKeys.begin())->rotation;
			}

			matrix4 transformM = glm::translate(matrix4(1.0f), position);
			matrix4 scalingM = glm::scale(matrix4(1.0f), scaling);
			matrix4 rotationM = glm::toMat4(rotation);

			localMatrix = transformM * rotationM * scalingM;
		}
	}

	matrix4 finalTransform = parent * localMatrix;
	
	// If this node has a name that matches a bone, create that matrix instead
	auto it = mc->mesh->children[0]->bones.find(node->name);
	if (it != mc->mesh->children[0]->bones.end()) {
		mc->boneMatrix[node->name] = /* inverseGlobalTransform * */ finalTransform * it->second->offsetMatrix;
	}

	// Go deeper
	for (auto it = node->children.begin(); it != node->children.end(); it++) {
		ProcessNodeHierarchy(*it, mc, currentFrame, finalTransform);
	}
}