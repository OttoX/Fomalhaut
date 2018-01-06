#pragma once

#include <vector>
#include <unordered_map>
#include "ecs_define.h"
#include "ecs_functional.h"

#include "entity.h"
#include "base_system.h"
#include "component_pool.h"

namespace ecs
{
	class Entity;
	class EntityAdmin
	{
	private:
		std::vector<BaseSystem*> systems_;
		std::unordered_map<EntityID, Entity*> entities_;
		ComponentPool component_pool_;

	public:
		EntityAdmin() = default;
		~EntityAdmin();
		void Update(float time_step);

		template<class S>
		S& CreateSystem();
		template<class S>
		void RemoveSystem();
		template<class S>
		bool ExistSystem();

		template<class E>
		Entity& CreateEntity();
		Entity* FindEntity(EntityID eid);
		void DestroyEntity(EntityID eid);

		//you can use your entity pool to generate eid
		EntityID GenerateEntityID()
		{
			static EntityID start_id = 0;
			return ++start_id;
		}

		ComponentVector& GetAllComponents(index_t id);
	private:
		void DestoryAllSysytems();
		void DestroyAllEntities();
	};

	template<class S>
	S& EntityAdmin::CreateSystem()
	{
		ECS_ASSERT_IS_SYSTEM(S);
		ECS_ASSERT(!ExistSystem<S>(), "System already exists");
		index_t system_index = details::SystemIndex::index<S>();
		if (system_index >= systems_.size()) {
			systems_.resize(system_index + 1, nullptr);
		}
		S* sys = new S(this);
		systems_[system_index] = sys;
		return *sys;
	}

	template<class S>
	void EntityAdmin::RemoveSystem()
	{
		ECS_ASSERT_IS_SYSTEM(S);
		ECS_ASSERT(ExistSystem<S>(), "System does not exist");
		delete systems_[details::SystemIndex::index<S>()];
		systems_[details::SystemIndex::index<S>()] = nullptr;
	}

	template<class S>
	bool EntityAdmin::ExistSystem()
	{
		ECS_ASSERT_IS_SYSTEM(S);
		return systems_.size() > details::SystemIndex::index<S>() && systems_[details::SystemIndex::index<S>()] != nullptr;
	}

	template<class E>
	Entity& ecs::EntityAdmin::CreateEntity()
	{
		ECS_ASSERT_IS_ENTITY(E);
		EntityID eid = GenerateEntityID();
		Entity* ent = new E(component_pool_, eid);
		entities_.insert(std::make_pair(eid, ent));
		return *ent;
	}

}