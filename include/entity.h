#pragma once

#include <map>
#include "ecs_functional.h"
#include "component_pool.h"

namespace ecs
{
    class EntityAdmin;
    class Entity
    {
	private:
		EntityAdmin* entity_admin_;
		EntityID eid_;
        std::map<index_t, BaseComponent*> components_;

    public:
        Entity(EntityAdmin* admin, EntityID eid);
        virtual ~Entity();

        template <typename T, typename... TArgs>
        auto Add(TArgs&&... args) -> Entity&;

        template <typename Arg>
        auto Remove() -> Entity&;
		template <typename Arg0, typename... Args>
		auto Remove() -> typename std::enable_if<sizeof...(Args) != 0, Entity&>::type;

        template <typename T, typename... TArgs>
        auto Replace(TArgs&&... args) -> Entity&;

        template <typename T>
        auto Get() const -> T*;
		template <typename... Args>
		auto Get() const -> typename std::enable_if< (sizeof...(Args) != 1), std::tuple<Args*...> > ::type
		{
			using ValueType = std::tuple<Args*...>;
			if (Has<Args...>())
			{
				return ValueType(Get<Args>()...);
			}
			return ValueType();
		}

		template <typename Arg>
		bool Has() const;
		template <typename Arg0, typename... Args>
		auto Has() const -> typename std::enable_if<sizeof...(Args) != 0, bool>::type;
    private:
        Entity& AddComponent(const index_t index, BaseComponent* component);
        Entity& RemoveComponent(const index_t index);
        Entity& ReplaceComponent(const index_t index, BaseComponent* component);
        BaseComponent* GetComponent(const index_t index) const;
        bool HasComponent(const index_t index) const;
        void Destroy();
        void ReplaceWith(const index_t index, BaseComponent* replacement);

		void DestroyAllComponent();
    };

}