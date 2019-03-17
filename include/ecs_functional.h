#pragma once

#include "ecs_define.h"
#include <type_traits>
#include <tuple>

namespace ecs
{
    namespace details
    {
		template<class...> struct conjunction : std::true_type { };
		template<class B1> struct conjunction<B1> : B1 { };
		template<class B1, class... Bn>
		struct conjunction<B1, Bn...>
			: std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};
		template<class... B>
		constexpr bool conjunction_v = conjunction<B...>::value;


		template <typename T>
		struct remove_const_and_reference
		{
			using type = typename std::remove_reference<typename std::remove_const<T>::type>::type;
		};

		template <typename F, typename Tuple, bool Done, size_t Total, size_t... N>
		struct apply_impl {
			static bool call(F f, Tuple&& t)
			{
				return apply_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(
					f, std::forward<Tuple>(t));
			}
		};

		template <typename F, typename Tuple, size_t Total, size_t... N>
		struct apply_impl<F, Tuple, true, Total, N...> {
			static bool call(F f, Tuple&& t) { return f(std::get<N>(std::forward<Tuple>(t))...); }
		};

		// user invokes this
		template <typename F, typename Tuple>
		bool apply(F f, Tuple&& t)
		{
			typedef typename std::decay<Tuple>::type ttype;
			return details::apply_impl<F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(
				f, std::forward<Tuple>(t));
		}

		//template <class F, class Tuple, std::size_t... I>
		//constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
		//{
		//	return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
		//}
		//template <class F, class Tuple>
		//constexpr decltype(auto) apply(F&& f, Tuple&& t)
		//{
		//	return apply_impl(
		//		std::forward<F>(f), std::forward<Tuple>(t),
		//		std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
		//}

		///--------------------------------------------------------------------
		/// function_traits is used to determine function properties
		/// such as parameter types (arguments) and return type.
		///--------------------------------------------------------------------
		template<typename T>
		struct function_traits
			: public function_traits<decltype(&T::operator())> {
		};
		// For generic types, directly use the result of the signature of its 'operator()'

		template<typename ClassType, typename ReturnType, typename... Args>
		struct function_traits<ReturnType(ClassType::*)(Args...) const>
			// we specialize for pointers to member function
		{
			enum { arg_count = sizeof...(Args) };

			typedef ReturnType return_type;

			template<size_t i>
			struct arg_t {
				typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
			};
			template<size_t i>
			using arg = typename arg_t<i>::type;

			template<size_t i>
			struct arg_remove_ref_t {
				typedef typename std::remove_reference<arg<i>>::type type;
			};
			template<size_t i>
			using arg_remove_ref = typename arg_remove_ref_t<i>::type;

			typedef std::tuple<Args...> args;
		};
        template <typename T>
        struct is_callable {
            template <typename U>
            static char test(decltype(&U::operator()));

            template <typename U>
            static int test(...);

            enum { value = sizeof(test<T>(0)) == sizeof(char) };
        };

        struct SystemIndex {
            template <typename S>
            static index_t index()
            {
                //ECS_ASSERT_IS_SYSTEM(S);
                static index_t idx = count()++;
                return idx;
            }
            static index_t& count()
            {
                static index_t counter = 0;
                return counter;
            }
        };

		struct ComponentIndex {
			template <typename C>
			static index_t index()
			{
				//ECS_ASSERT_IS_COMPONENT(C);
				static index_t idx = count()++;
				return idx;
			}
			static index_t& count()
			{
				static index_t counter = 0;
				return counter;
			}
		};
    }
}