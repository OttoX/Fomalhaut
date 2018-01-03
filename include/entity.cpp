
#include "entity.h"
#include "entity_admin.h"
#include "base_component.h"

using namespace ecs;


template <typename T, typename... TArgs>
auto Entity::Add(TArgs&&... args) -> Entity&
{
	ComponentPool& pool = entity_admin_->GetComponentPool();
	return AddComponent(details::ComponentIndex::index<T>(), pool.CreateComponent<T>(std::forward<TArgs>(args)...));
}

template <typename Arg>
auto Entity::Remove() -> Entity&
{
	return RemoveComponent(details::ComponentIndex::index<Arg>());
}
template <typename Arg0, typename... Args>
auto Entity::Remove() -> typename std::enable_if<sizeof...(Args) != 0, Entity&>::type
{
	return RemoveComponent(details::ComponentIndex::index<Arg0>()), Remove<Args...>();
}

template <typename T, typename... TArgs>
auto Entity::Replace(TArgs&&... args) -> Entity&
{
	ComponentPool& pool = entity_admin_->GetComponentPool();
	return ReplaceComponent(details::ComponentIndex::index<T>(),
		pool.CreateComponent<T>(std::forward<TArgs>(args)...));
}

template <typename T>
auto Entity::Get() const -> T*
{
	return static_cast<T*>(GetComponent(details::ComponentIndex::index<T>()));
}

template <typename Arg>
bool Entity::Has() const
{
	return HasComponent(details::ComponentIndex::index<Arg>());
}

template<typename Arg0, typename ...Args>
auto Entity::Has() const -> typename std::enable_if<sizeof ...(Args) != 0, bool>::type
{
	return HasComponent(details::ComponentIndex::index<Arg0>()) && Has<Args...>();
}

Entity::Entity(EntityAdmin* admin, EntityID eid) : entity_admin_(admin), eid_(eid) {}

Entity::~Entity() 
{
	DestroyAllComponent();
}

Entity& Entity::AddComponent(const index_t index, BaseComponent* component) 
{ 
	if (HasComponent(index))
	{
		throw std::runtime_error("Error, cannot add component to entity, component already exists");
	}
	components_[index] = component;
	component->SetOwner(this);
	return *this;
}

Entity& Entity::RemoveComponent(const index_t index) 
{ 
	if (!HasComponent(index))
	{
		throw std::runtime_error("Error, cannot remove component to entity, component not exists");
	}

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
		 ComponentPool& pool = entity_admin_->GetComponentPool();
		 pool.RemoveComponent(index, prev_component);
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
	ComponentPool& pool = entity_admin_->GetComponentPool();
	for (const auto& kv : components_)
	{
		pool.RemoveComponent(kv.first, kv.second);
	}
}
