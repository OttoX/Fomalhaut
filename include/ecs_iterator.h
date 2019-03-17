#pragma once

#include <functional>
#include <tuple>
#include "ecs_functional.h"
#include "entity_admin.h"
namespace ecs
{

	template<typename T>
	struct IsComponent : std::integral_constant<bool, std::is_base_of<BaseComponent, T>::value && !std::is_same<BaseComponent, T>::value> {};
	template<typename T>
	constexpr bool IsComponent_v = IsComponent<T>::value;

	template<typename T>
	struct IsComponentPointer : std::integral_constant<bool, IsComponent_v<std::remove_pointer<T>> && !std::is_pointer<T>::value> {};
	template<typename T>
	constexpr bool IsComponentPointer_v = IsComponentPointer<T>::value;

	template <typename... Args>
	class ComponentItr
	{
		static_assert(details::conjunction_v<IsComponent<Args>... > && (sizeof...(Args) > 0), "invalid argument type!");
	private:
		class ItemIterator;
		using Iterator = ItemIterator;
		using TagDispatchType = std::conditional_t<(sizeof...(Args) == 1), std::true_type, std::false_type>;
		using Pr = typename std::function<bool(std::add_pointer_t<Args>...)>;

		EntityAdmin* admin_;
		Pr pred_;

	public:
		ComponentItr(EntityAdmin* admin)
			: admin_(admin)
		{
		}
		ComponentItr(EntityAdmin* admin, Pr&& pred)
			: admin_(admin), pred_(std::move(pred))
		{
		}

		Iterator begin() {
			return Iterator(admin_, pred_, true);
		}
		Iterator end() {
			return Iterator(admin_, pred_, false);
		}
	private:
		class ItemIterator
		{
		private:
			Pr pred_;
			ComponentVector& component_vector_;
			ComponentVector::const_iterator it_;
			ComponentVector::const_iterator end_;
		public:
			ItemIterator(EntityAdmin* admin, Pr& pred, bool is_begin = true)
				: pred_(pred)
				, component_vector_(GetLeastComponentVector<Args...>(admin))
			{
				end_ = component_vector_.end();
				if (is_begin) {
					it_ = component_vector_.begin();
				}
				else
				{
					it_ = end_;
				}
				find_next(TagDispatchType());
			}

			bool operator!=(const ItemIterator& rhs) const { return It() != rhs.It(); }
			ComponentVector::const_iterator It() const
			{
				return it_;
			}

			decltype(auto) operator*()
			{
				auto ent = (*it_)->Owner();
				assert(ent);
				return ent->Get<Args...>();
			}
			ItemIterator& operator++()
			{
				++it_;
				find_next(TagDispatchType());
				return *this;
			}

		private:
			template<typename U>
			ComponentVector& GetLeastComponentVector(EntityAdmin* admin, ComponentVector& compare_vector)
			{
				auto& component_vector = admin->GetAllComponents(details::ComponentIndex::index<U>());
				return compare_vector.size() <= component_vector.size() ? compare_vector : component_vector;
			}

			template<typename U, typename... Us>
			auto GetLeastComponentVector(EntityAdmin* admin, ComponentVector& compare_vector) -> std::enable_if_t<(sizeof...(Us) != 0), ComponentVector& >
			{
				auto& component_vector = admin->GetAllComponents(details::ComponentIndex::index<U>());
				return GetLeastComponentVector<Us...>(admin, compare_vector.size() <= component_vector.size() ? compare_vector : component_vector);
			}
			template<typename U>
			ComponentVector& GetLeastComponentVector(EntityAdmin* admin)
			{
				return admin->GetAllComponents(details::ComponentIndex::index<U>());
			}
			template<typename U, typename... Us>
			std::enable_if_t<sizeof...(Us) != 0, ComponentVector&> GetLeastComponentVector(EntityAdmin* admin)
			{
				auto& component_vector = admin->GetAllComponents(details::ComponentIndex::index<U>());
				return GetLeastComponentVector<Us...>(admin, component_vector);
			}
		private:
			void find_next(std::false_type&& t) {
				while (it_ != end_)
				{
					auto ent = (*it_)->Owner();
					if (ent && ent->Has<Args...>() && (!pred_ || (pred_ && details::apply(pred_, ent->Get<Args...>()))))
					{
						break;
					}
					++it_;
				}
			}

			void find_next(std::true_type&& t) {
				while (it_ != end_ && pred_ && !pred_((*it_)->Owner()->Get<Args...>()))
				{
					++it_;
				}
			}
		};
	};
}