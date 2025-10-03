
#include <iostream>
#include <vector>
using namespace std;
int main() {
    vector<int> v;
    v.pop_back();
    cout << v.back() << endl;
    cout << (unsigned long long)(-1) << endl;
    unsigned int a = 5, b = 10;
    __asm__("movl $1, %0" : "=g"(a) : "r"(b));
    cout << a << " " << b << endl;
    return 0;
}