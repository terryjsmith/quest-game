
#include <Render/QGAnimatedMeshComponent.h>
#include <Core/QGTimeSystem.h>
#include <Core/QGApplication.h>
#include <IO/QGResourceSystem.h>

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

	std::string activeAnim = "";
	if (activeAnimation) {
		activeAnim = activeAnimation->name + "|" + activeAnimation->resource->path + "/" + activeAnimation->resource->filename;
	}
	record->Set("active", activeAnim);
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

	// Parse animation name from filename
	std::string animationName = active.substr(0, active.find_last_of("|"));
	std::string filename = active.substr(active.find_last_of("|") + 1);

	// Make sure it's loaded
	auto it = this->animations.find(animationName);
	if (it == this->animations.end()) {
		QGResourceSystem* resourceSystem = GetQGSystem<QGResourceSystem>();
		QGAnimation* animation = (QGAnimation*)resourceSystem->Load(filename, "Animation");
		this->animations[animationName] = animation;
	}

	// Ensure we are playing this animation
	bool loop = record->Get("loop").AsBool();
	uint64_t start = record->Get("start").AsUInt64();

	// If playing, play
	this->Play(animationName, loop, start);
}