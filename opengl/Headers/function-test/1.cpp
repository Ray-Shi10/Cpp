#include <iostream>
#include <functional>
#include <utility>
using namespace std;

template <typename _Func_t>
class Function {};

template <typename _Ret_t, typename... _Args_t>
class Function<_Ret_t(_Args_t...)> {
public:
    using _Func_t = _Ret_t(_Args_t...);
    _Func_t* _func;
    Function(_Func_t *func) : _func(func) {}
    template <typename _Func_t_0>
    Function(_Func_t_0 *func) {
        _func = (std::forward<_Func_t*>(func));
    }/*
    template <typename ..._Args_t_0>
    _Ret_t operator()(_Args_t_0 ...args) {
        return _func(std::forward<_Args_t_0>(args)...);
    }/*/
    _Ret_t operator()(_Args_t ...args) {
        return (std::forward<_Func_t*>(_func))(std::forward<_Args_t>(args)...);
    }
};

typedef void(*voidfunc)();
template <typename F>
voidfunc lam2fun(F f) {
    static auto lam = f;
    return [](){ lam(); };
}

int main() {
    //std::forward<int>(1);
    typedef void(*func_t)(int);
    int a=1;
    std::function<void()> stdf([a]() {
        cout << "Hello, World!" << endl;
    });
    //stdf();
    auto func1 = []() {
        cout << "Hello, World!----" << endl;
    };
    auto func2 = []() {
        cout << "Hello, World!-------------" << endl;
    };
    std::function<void()> stdf1(func1);
    const auto func_ptr = stdf1.target<void(*)()>();
    if (func_ptr) {
        (*func_ptr)();
    } else {
        cout << "Failed to convert to function pointer." << endl;
    }
    lam2fun(func1)();
    lam2fun(func2)();
    lam2fun(func1)();
    lam2fun(func2)();
    //(*lambda[](int)->void) a;
    //func.operator func_t();
    //Function<void(int)> f(func); f(1);
    return 0;
}