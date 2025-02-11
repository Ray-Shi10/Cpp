#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <iostream>
#include <functional>
#include <type_traits>

template <typename __ReturnType, typename... __Args>
class _function_base {
public:
	enum { arglen = sizeof...(__Args) };
    using args_type = std::tuple<__Args...>;
	using return_type = __ReturnType;
	using function_type = return_type(__Args...);
	using pointer_type = function_type*;
	using stl_function_type = std::function<function_type>;
	using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<__Args>>...>;
	//using bare_tuple_type = std::tuple<std::remove_const_t<std::remove_reference_t<__Args>>...>;
	template <size_t I>
	struct args {
		static_assert(I < arglen, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, std::tuple<__Args...>>::type;
	};

    pointer_type func;
    _function_base() : func(nullptr) {}
    _function_base(pointer_type f) : func(f) {}
    template <typename _Func>
    _function_base(_Func f) {
        static auto lam = (decltype(f))(f);
        func = ([](__Args... _args) -> return_type { return static_cast<return_type>(lam(_args...)); });
    }
    return_type operator()(__Args... args) const {
        return func(args...);
    }
    return_type apply(__Args... args) const {
        if(!func) {
            std::cerr << "Function is not callable" << std::endl;
            return return_type();
        } else {
            return func(args...);
        }
    }/*
    return_type operator()(tuple_type args) const {
        return std::apply(func, args);
    }
    return_type apply(tuple_type args) const {
        if(!func) {
            std::cerr << "Function is not callable" << std::endl;
            return return_type();
        } else {
            return std::apply(func, args);
        }
    }*/
    operator stl_function_type() const {
        return stl_function_type(func);
    }
    operator pointer_type() const {
        return func;
    }
};

//#define _memberfuncs public: \
    template <typename _Func> function(_Func f) : _function_base<_Func>(f) {} \
    return_type operator()(__Args... args) { return (*this)(args...) }

template <typename _Func> struct _function_tmp;

// 普通函数
template <typename ReturnType, typename... Args>
struct _function_tmp<ReturnType(Args...)> { public:
using type = _function_base<ReturnType,Args...>; };
// 函数指针
template <typename ReturnType, typename... Args>
struct _function_tmp<ReturnType(*)(Args...)> { public:
using type = _function_base<ReturnType,Args...>; };

// std::function
template <typename _Func>
struct _function_tmp<std::function<_Func>> { public:
using type = _function_base<_Func>; };

// 成员函数
#define FUNCTION(...) \
template <typename ReturnType, typename ClassType, typename... Args> \
struct _function_tmp<ReturnType(ClassType::*)(Args...) __VA_ARGS__> { public: \
using type = _function_base<ReturnType,Args...>; }; \

FUNCTION()
FUNCTION(const)
FUNCTION(volatile)
FUNCTION(const volatile)
#undef FUNCTION

// 函数对象
template <typename Callable>
struct _function_tmp : _function_tmp<decltype(&Callable::operator())> {};

template <typename _Func>
using function_t = typename _function_tmp<_Func>::type;

template <typename _Func>
auto function(_Func f) { return function_t<_Func>(f); }

#endif