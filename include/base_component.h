#pragma once

#include "ecs_functional.h"
//#include "entity.h"

namespace ecs
{
	class Entity;
    class BaseComponent
    {
    protected:
		Entity* ent_{ nullptr };

    public:
		BaseComponent() = default;
        virtual ~BaseComponent() = default;

		void SetOwner(Entity* ent) { ent_ = ent; }
		Entity* Owner() { return ent_; }

        template <typename C>
        C* As();
        template <typename C>
        C const* As() const;

		template<class C>
		C* Sibling();

	private:
		BaseComponent* SiblingComponent(const index_t index);
    };

	template <typename C>
	C* BaseComponent::As()
	{
		ECS_ASSERT_IS_COMPONENT(C);
		return static_cast<C*>(this);
	}

	template <typename C>
	C const* BaseComponent::As() const
	{
		ECS_ASSERT_IS_COMPONENT(C);
		return static_cast<C*>(this);
	}

	template<class C>
	C* BaseComponent::Sibling()
	{
		ECS_ASSERT_IS_COMPONENT(C);
		BaseComponent* component = SiblingComponent(details::ComponentIndex::index<C>());
		return component ? component->As<C>() : nullptr;
	}
}
