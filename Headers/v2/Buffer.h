#ifndef _BUFFER_H_
#define _BUFFER_H_

//#include "GLM.h"
#include <vector>

template <typename ..._Args>
class Buffer {
private:
    template <typename __T> using vector = std::vector<__T>;
public:
    using pack = std::tuple<_Args...>;
    vector<pack> data;
    Buffer() = default;
    Buffer(vector<pack> const&data) : data(data) {}

    size_t size() const { return data.size(); }
    size_t sizeByte() const { return data.size() * sizeof(pack); }
    pack const*dataPtr() const { return data.data(); }
    void clear() { data.clear(); }
    void push_back(pack const& p) { data.push_back(p); }
    void push_back(_Args const&...args) { data.push_back(pack(args...)); }

    static constexpr size_t packSize = sizeof(pack);
    static constexpr size_t packLen  = sizeof...(_Args);
    template <size_t i> using kthType = typename std::tuple_element<i, pack>::type;
};

#endif // _BUFFER_H_