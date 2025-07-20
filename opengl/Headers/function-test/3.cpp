#include <iostream>

#include <functional>

template<typename T>
struct function_traits;

// 普通函数
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)> {
    using Args_t = std::tuple<Args...>;
	enum { arity = sizeof...(Args) };
	using return_type = ReturnType;
	using function_type = ReturnType(Args...);
	using stl_function_type = std::function<function_type>;
	using pointer_type = ReturnType(*)(Args...);

	template<size_t I>
	struct args {
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
	};

	using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
	using bare_tuple_type = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
};

// 函数指针
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)> : function_traits<ReturnType(Args...)> {};

// std::function
template <typename ReturnType, typename... Args>
struct function_traits<std::function<ReturnType(Args...)>> : function_traits<ReturnType(Args...)> {};

// 成员函数
#define FUNCTION_TRAITS(...) \
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{};

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)
#undef FUNCTION_TRAITS

// 函数对象
template <typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())> {};

template <typename _Callable> class function;

template <
//typename _Func_t, 
typename _Ret_t,
typename ..._Args_t
>
class function<_Ret_t(_Args_t...)> {
public:
    using _Func_t = _Ret_t(_Args_t...);
    using _Func_t_tup = _Ret_t(std::tuple<_Func_t>);
    //using _Ret_t = typename function_traits<_Func_t>::return_type;
    //using _Args_t = typename function_traits<_Func_t>::Args_t;
    using _Args_tup = std::tuple<_Args_t...>;
    _Func_t_tup *_func;
    function(_Func_t *func) {
        _func = func;
    }
    template <typename _Func_t_0>
    function(_Func_t_0 func) {
        static auto lam = decltype(func)(func);
        _func = ([](_Args_tup args) -> typename function_traits<_Func_t_0>::return_type { return lam(args...); });
    }//*
    _Ret_t operator()(_Args_t ...args) {
        return (std::forward<_Func_t*>(_func))(_Args_tup(std::forward<_Args_t>(args)));
    }//*/
    operator _Func_t*() {
        return _func;
    }
};

template <typename Func>
auto make_function(Func func) {
    //return function<Func>(func);
}

int main() {
    int b=-1, c=1;
    auto f = function<int(int)>([b, &c, d=1](int a) mutable {
        std::cout << "\na: " << a << std::endl;
        std::cout << "b: " << b << std::endl;
        std::cout << "c: " << c << std::endl;
        std::cout << "d: " << d << std::endl;
        std::cout << "-------------";
        c += a;
        return c;
    });
    std::cout << f(1);
    std::cout << f(2);
    c++;
    std::cout << f(-3);
    c=0;
    std::cout << f(4);
}