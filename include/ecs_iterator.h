#pragma once

#include <functional>
#include <tuple>
#include "ecs_functional.h"
#include "entity_admin.h"
namespace ecs
{
	template <typename T>
	class ComponentItr
	{
	private:
		class ComponentIterator;
		using RawType = typename details::remove_const_and_reference<T>::type;
		using PointerType = RawType*;
		using Const_PointerType = const PointerType;
		using Iterator = ComponentIterator;
		using Pr = typename std::function<bool(Const_PointerType)>;

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
		class ComponentIterator
		{
		private:
			EntityAdmin* admin_;
			Pr pred_;
			size_t cursor_;
			size_t size_;
		public:
			ComponentIterator(EntityAdmin* admin, Pr& pred, bool is_begin = true) : admin_(admin), pred_(pred), cursor_(0)
			{
				size_ = admin_->GetAllComponents(details::ComponentIndex::index<T>()).size();
				if (!is_begin) {
					cursor_ = size_;
				}
				find_next();
			}
			size_t index() const { return cursor_; }
			PointerType Component() {
				ComponentVector& vec = admin_->GetAllComponents(details::ComponentIndex::index<T>());
				return vec.at(index())->template As<RawType>();
			}
			bool operator!=(const ComponentIterator& rhs) const { return !(this->index() == rhs.index()); }
			PointerType operator*() { return Component(); }
			ComponentIterator& operator++()
			{
				++cursor_;
				find_next();
				return *this;
			}
		private:
			void find_next() {
				while (cursor_ < size_ && pred_ && !pred_(Component()))
				{
					++cursor_;
				}
			}
		};
	};

	template <typename... Args>
	class TupleItr
	{
	private:
		static_assert(sizeof...(Args) != 0, "invalid argument count! need more than one argument!");
		static_assert(sizeof...(Args) != 1, "use ComponentItr<T> to iterate!");
		class TupleIterator;
		using Iterator = TupleIterator;
		using Pr = typename std::function<bool(const typename details::remove_const_and_reference<Args>::type* ...)>;
		using ValueType = std::tuple<typename details::remove_const_and_reference<Args>::type* ...>;
		using FirstArgType = typename std::remove_pointer<typename std::tuple_element<0, ValueType>::type>::type;

		EntityAdmin* admin_;
		Pr pred_;
		
	public:
		TupleItr(EntityAdmin* admin)
			: admin_(admin)
		{
		}
		TupleItr(EntityAdmin* admin, Pr&& pred)
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
		class TupleIterator
		{
		private:
			EntityAdmin* admin_;
			Pr pred_;
			size_t cursor_;
			size_t size_;
		public:
			TupleIterator(EntityAdmin* admin, Pr& pred, bool is_begin = true) : admin_(admin), pred_(pred), cursor_(0)
			{
				size_ = admin_->GetAllComponents(details::ComponentIndex::index<FirstArgType>()).size();
				if (!is_begin) {
					cursor_ = size_;
				}
				find_next();
			}
			size_t index() const { return cursor_; }
			FirstArgType* Component() {
				ComponentVector& vec = admin_->GetAllComponents(details::ComponentIndex::index<FirstArgType>());
				return vec.at(index())->template As<FirstArgType>();
			}
			bool operator!=(const TupleIterator& rhs) const { return !(this->index() == rhs.index()); }
			ValueType operator*() 
			{ 
				Entity* ent = Component()->Owner();
				if (ent)
				{
					return ValueType(ent->Get<Args...>());
				}
				throw std::runtime_error("Error, TupleItr operator '*' must not be nullptr value");
				return ValueType();
			}
			TupleIterator& operator++()
			{
				++cursor_;
				find_next();
				return *this;
			}
		private:
			void find_next() {
				while (cursor_ < size_)
				{
					Entity* ent = Component()->Owner();
					if (ent && ent->Has<Args...>() && (!pred_ || (pred_ && details::apply(pred_, ent->Get<Args...>()))))
					{
						break;
					}
					++cursor_;
				}
			}
		};
	};
}