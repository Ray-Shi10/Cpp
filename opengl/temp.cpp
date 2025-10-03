#include <iostream>
#include <cmath>
using namespace std;
#define int unsigned long long
signed main() {
  float a=8;
  // int n = 50;
  for(int i=1; i<50; i++) {
    // int c = (i+1)*(i+2)*(i+3) / 6 * 8;
    int c = (1<<i) * 2;
    a = (i+3)*a + float(1<<i)*(i-3);
    a /= i;
    printf("%02llu: %010llu - %010.f = %010.f  |  %015.3f\n", i+1, c, a, c-a, (c-a)/i/i/i);
  }
  return 0;
}