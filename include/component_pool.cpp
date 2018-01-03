#include "component_pool.h"
#include <algorithm>
using namespace ecs;

void ComponentPool::RemoveComponent(index_t id, BaseComponent* component)
{
	if (!component)
	{
		return;
	}
	ComponentVector& component_vec = GetAllComponents(id);
	auto it = std::find(component_vec.begin(), component_vec.end(), component);
	assert(it != component_vec.end());
	if (it != component_vec.end())
	{
		std::swap(*it, component_vec.back());

		delete component_vec.back();

		component_vec.pop_back();
	}
}

ComponentVector& ComponentPool::GetAllComponents(index_t id)
{
	return component_pools_[id];
}
