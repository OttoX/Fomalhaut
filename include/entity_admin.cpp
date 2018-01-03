#include "entity_admin.h"

using namespace ecs;

EntityAdmin::~EntityAdmin()
{
	DestoryAllSysytems();
	DestroyAllEntities();
}

void ecs::EntityAdmin::Update(float time_step)
{
	for (BaseSystem* s : systems_) {
		s->Update(time_step);
	}
}

Entity * ecs::EntityAdmin::FindEntity(EntityID eid)
{
	auto it = entities_.find(eid);
	return (it == entities_.end()) ? nullptr : it->second;
}

void ecs::EntityAdmin::DestroyEntity(EntityID eid)
{
	auto it = entities_.find(eid);
	if (it != entities_.end())
	{
		delete it->second;
	}
}

void EntityAdmin::DestoryAllSysytems()
{
	for (BaseSystem* sys : systems_)
	{
		delete sys;
	}
	systems_.clear();
}

void EntityAdmin::DestroyAllEntities()
{
	for (auto& kv : entities_)
	{
		delete kv.second;
	}
	entities_.clear();
}
