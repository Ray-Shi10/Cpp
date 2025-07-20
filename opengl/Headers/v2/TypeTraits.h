#ifndef _TYPE_TRAITS_H_
#define _TYPE_TRAITS_H_

namespace std {
    template <typename _Tuple>
    struct tuple_kthType {
        template <size_t I>
        using type = typename std::tuple_element<I, _Tuple>::type;
    };
};

#endif //_TYPE_TRAITS_H_