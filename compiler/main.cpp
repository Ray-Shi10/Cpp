#include <iostream>
using std::cout;

using ull = unsigned long long;
constexpr ull _hash(ull &h, const char *a) {
  for(int i = 0; a[i]; ++i) h = (h<<5) - h + a[i]; return h;
}
template <typename...Args>
constexpr ull hash(Args...args) {
  ull h = 0;
  (void)std::initializer_list<int>{(_hash(h, args), 0)...};
  return h;
}

class expr {
public:
  expr(expr) {
  }
};

template <const char *...args>
class A {};
int main() {
  // cout def("Hello", "World", '\n')"";
}