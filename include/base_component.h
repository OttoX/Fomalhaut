#pragma once

#include "ecs_functional.h"

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
    };
}
