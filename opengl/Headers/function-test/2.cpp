#include <iostream>
//#include <functional>
using namespace std;

template <typename _Func_t>
class lam2fun {};

template <typename _Ret_t, typename... _Args_t>
class lam2fun<_Ret_t(_Args_t...)> {
public:
    using _Func_t = _Ret_t(_Args_t...);
    _Func_t* _func;
    lam2fun(_Func_t *func) : _func(func) {}
    template <typename _Func_t_0>
    lam2fun(_Func_t_0 func) {
        static auto lam = decltype(func)(func);
        //_func = (std::forward<_Func_t*>(func));
        _func = [](_Args_t... args) { lam(args...); };
    }//*
    _Ret_t operator()(_Args_t ...args) {
        return (std::forward<_Func_t*>(_func))(std::forward<_Args_t>(args)...);
    }//*/
    operator _Func_t*() {
        return _func;
    }
};

int main() {
    typedef void(*func_t)(int);
    int a=1;
    auto func1 = [&]() {
        cout << "Hello, World!    " << a++ << endl;
    };
    lam2fun<void()> stdf1(func1);
    lam2fun<void()> stdf2(func1);
    stdf1();
    stdf2();
    a++;
    stdf1();
    stdf2();
    //(*lambda[](int)->void) a;
}