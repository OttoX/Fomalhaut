#pragma once

#include <cstdint>
#include <vector>
#include <type_traits>
#include <cassert>

namespace ecs
{

	using EntityID = uint32_t;
	using index_t = uint32_t;
	using ComponentIndexList = std::vector<index_t>;

#ifndef ECS_ASSERT
#define ECS_ASSERT(Expr, Msg) assert(Expr && Msg)
#pragma message("ECS_ASSERT defined!")
#endif

#define ECS_ASSERT_IS_CALLABLE(T) \
    \
static_assert(details::is_callable<T>::value, "Provide a function or lambda expression");

#define ECS_ASSERT_IS_ENTITY(T) static_assert(std::is_same<class Entity, T>::value, #T " is not entity");

#define ECS_ASSERT_IS_COMPONENT(T)                                                                      \
    static_assert((std::is_base_of<BaseComponent, T>::value && !std::is_same<BaseComponent, T>::value), \
                  "Class type must be derived from BaseComponent");

#define ECS_ASSERT_IS_SYSTEM(T)                                                                   \
    static_assert((std::is_base_of<BaseSystem, T>::value && !std::is_same<BaseSystem, T>::value), \
                  #T " must be derived from BaseSystem");

}