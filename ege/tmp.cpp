#include <iostream>
#include <ctime>
using namespace std;
int main() {
    clock_t start = clock();
    // _sleep(1000);
    cout << "Time: " << (clock()-start)*1.0/CLOCKS_PER_SEC << endl;
    return 0;
}