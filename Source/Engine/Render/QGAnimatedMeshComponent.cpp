
#include <Render/QGAnimatedMeshComponent.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGApplication.h>

void QGAnimatedMeshComponent::Play(std::string animationName, bool loop, uint64_t startTick) {
	auto it = animations.find(animationName);
	QGASSERT(it != animations.end(), "No animation named %s to play.", animationName.c_str());

	QGTimeSystem* timeSystem = GetQGSystem<QGTimeSystem>();
	activeAnimation = it->second;
	looping = loop;
	startTime = (startTick == 0) ? timeSystem->Tick() : startTick;
}

void QGAnimatedMeshComponent::Stop() {
	activeAnimation = 0;
	looping = false;
	startTime = 0;
}

void QGAnimatedMeshComponent::AddAnimation(std::string name, QGAnimation* animation) {
	animations[name] = animation;
	animation->name = name;
}

void QGAnimatedMeshComponent::Serialize(QGDataRecord* record) {
	QGMeshComponent::Serialize(record);

	record->Set("active", activeAnimation == 0 ? std::string("") : activeAnimation->name);
	record->Set("loop", looping);
	record->Set("start", startTime);
}

void QGAnimatedMeshComponent::Deserialize(QGDataRecord* record) {
	QGMeshComponent::Deserialize(record);

	// Check active animation
	std::string active = record->Get("active").AsString();

	// If empty, make sure we are not playing anything
	if (active.size() == 0) {
		this->Stop();
		return;
	}

	// Ensure we are playing this animation
	bool loop = record->Get("loop").AsBool();
	uint64_t start = record->Get("start").AsUInt64();
	this->Play(active, loop, start);
}