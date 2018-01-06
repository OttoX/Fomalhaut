#include "entity.h"

using namespace ecs;

BaseComponent* BaseComponent::SiblingComponent(const index_t index)
{
	return ent_ ? ent_->GetComponent(index) : nullptr;
}