#include "entity.h"

using namespace ecs;

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
	return ent_ ? ent_->Get<C>() : nullptr;
}
