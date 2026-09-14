
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
		double currentTime = ((currentTick - (*it)->startTime) / QG_TICKS_PER_SECOND);

		// Convert to current frame
		int currentFrame = (currentTime * animation->speed);

		// If we are past the end, and not looping, then stop
		if (currentFrame > animation->duration) {
			if ((*it)->looping == false) {
				(*it)->Stop();
				continue;
			}
			
			// Adjust time back to beginning
			currentFrame -= animation->duration;
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
			double scalingKey1 = transforms->scalingKeys.begin()->first;
			double scalingKey2 = transforms->scalingKeys.begin()->first;
			for (auto sit = transforms->scalingKeys.begin(); sit != transforms->scalingKeys.end(); sit++) {
				scalingKey2 = sit->first;
				if (scalingKey2 >= currentFrame) break;
				scalingKey1 = sit->first;
			}

			// Compute difference
			float difference = (currentFrame - scalingKey1) / (scalingKey2 - scalingKey1);

			// Interpolate
			vector3 scalediff = transforms->scalingKeys[scalingKey2].scaling - transforms->scalingKeys[scalingKey1].scaling;
			vector3 scaling = transforms->scalingKeys[scalingKey1].scaling + (scalediff * difference);

			// Next up: position
			double positionKey1 = transforms->translationKeys.begin()->first;
			double positionKey2 = transforms->translationKeys.begin()->first;
			for (auto tit = transforms->translationKeys.begin(); tit != transforms->translationKeys.end(); tit++) {
				positionKey2 = tit->first;
				if (positionKey2 >= currentFrame) break;
				positionKey1 = tit->first;
			}

			// Compute difference
			difference = (currentFrame - positionKey1) / (positionKey2 - positionKey1);

			// Interpolate
			vector3 posdiff = transforms->translationKeys[positionKey2].position - transforms->translationKeys[positionKey1].position;
			vector3 position = transforms->translationKeys[positionKey1].position + (posdiff * difference);

			// Finally, rotation
			double rotationKey1 = transforms->rotationKeys.begin()->first;
			double rotationKey2 = transforms->rotationKeys.begin()->first;
			for (auto rit = transforms->rotationKeys.begin(); rit != transforms->rotationKeys.end(); rit++) {
				rotationKey2 = rit->first;
				if (rotationKey2 >= currentFrame) break;
				rotationKey1 = rit->first;
			}

			// Compute difference
			difference = (currentFrame - rotationKey1) / (rotationKey2 - rotationKey1);

			// Interpolate
			quaternion rotation = glm::slerp(transforms->rotationKeys[rotationKey1].rotation, transforms->rotationKeys[rotationKey2].rotation, difference);

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