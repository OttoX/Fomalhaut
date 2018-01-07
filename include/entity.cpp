
#include "entity.h"
#include "base_component.h"

using namespace ecs;

Entity::Entity(ComponentPool& pool, EntityID eid) : pool_(pool), eid_(eid) {}

Entity::~Entity()
{
	DestroyAllComponent();
}

Entity& Entity::AddComponent(const index_t index, BaseComponent* component) 
{
	ECS_ASSERT(!HasComponent(index), "Error, cannot add component to entity, component already exists");
	components_[index] = component;
	component->SetOwner(this);
	return *this;
}

Entity& Entity::RemoveComponent(const index_t index) 
{
	ECS_ASSERT(HasComponent(index), "Error, cannot remove component to entity, component not exists");
	ReplaceWith(index, nullptr);
	return *this;
}

Entity& Entity::ReplaceComponent(const index_t index, BaseComponent* component) 
{
	if (HasComponent(index))
	{
		ReplaceWith(index, component);
	}
	else if (component != nullptr)
	{
		AddComponent(index, component);
	}

	return *this;
}

BaseComponent* Entity::GetComponent(const index_t index) const
{
	if (!HasComponent(index))
	{
		return nullptr;
	}

	return components_.at(index);
}

bool Entity::HasComponent(const index_t index) const 
{ 
	return (components_.find(index) != components_.end());
}

void Entity::Destroy() {}

void Entity::ReplaceWith(const index_t index, BaseComponent * replacement)
{
	BaseComponent* prev_component = GetComponent(index);
	if (prev_component == replacement)
	{
	}
	else
	{
		//onRemove
		pool_.RemoveComponent(index, prev_component);
		if (replacement == nullptr)
		{
			components_.erase(index);
		}
		else
		{
			components_[index] = replacement;
			components_[index]->SetOwner(this);
		}
	}
}

void Entity::DestroyAllComponent()
{
	for (const auto& kv : components_)
	{
		//onRemove()
		pool_.RemoveComponent(kv.first, kv.second);
	}
	components_.clear();
}
