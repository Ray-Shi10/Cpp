#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>

#include "base.h"

template <typename _T, glm::length_t _n, glm::length_t _m, glm::qualifier _q>
class __GetGLMType {
public:
    using type = glm::mat<_n, _m, _T, _q>;
};
template <typename _T, glm::length_t _n, glm::qualifier _q>
class __GetGLMType<_T, _n, 0, _q> {
public:
    using type = glm::vec<_n, _T, _q>;
};

template <typename _T_base, glm::length_t _n, glm::length_t _m, glm::qualifier _q>
class _Buffer_base {
    using _GetGLMType = __GetGLMType<_T_base, _n, _m, _q>;
public:
    using type = typename _GetGLMType::type;
    using baseType = _T_base;
    template <typename __T>
    using vector = std::vector<__T>;
    using vec = vector<baseType>;
    static constexpr glm::length_t n = _n;
    static constexpr glm::length_t m = _m;
    static constexpr glm::length_t size = _n * _m;
    static constexpr glm::qualifier q = _q;
    vec data;
    _Buffer_base() {}
    _Buffer_base(vec const&data): data(data) {}
    _Buffer_base(vector<type> const&data) {
        if(_m == 1) {
            for(auto const& d : data) {
                for(glm::length_t i = 0; i < _n; i++) {
                    this -> data.push_back(d[i]);
                }
            }
        } else {
            for(auto const& d : data) {
                for(glm::length_t i = 0; i < _n; i++) {
                    for(glm::length_t j = 0; j < _m; j++) {
                        this -> data.push_back(d[i][j]);
                    }
                }
            }
        }
    }
    std::size_t glmSize() const { return data.size() / size; }
    std::size_t rawSize() const { return data.size(); }
    baseType const*rawData() const { return data.data(); }
    type *glmData() const { return data.data(); }
};

template <typename _T>
class _Buffer {
public: using type = _Buffer_base<_T, 0, 0, glm::defaultp>;
};

template <typename _T, glm::length_t _n, glm::qualifier _q>
class _Buffer<glm::vec<_n, _T, _q>> {
public: using type = _Buffer_base<_T, _n, 0, _q>;
};
template <typename _T, glm::length_t _n, glm::length_t _m, glm::qualifier _q>
class _Buffer<glm::mat<_n, _m, _T, _q>> {
public: using type = _Buffer_base<_T, _n, _m, _q>;
};

template <typename _T>
using Buffer = typename _Buffer<_T>::type;

#endif // _BUFFER_H_