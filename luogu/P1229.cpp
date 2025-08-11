#include <iostream>
#include <string.h>
using namespace std;

int main() {
    char forward[30], backward[30];
    cin >> forward >> backward;
    const int len = strlen(forward);
    int cnt=0;
    for(int i=0; i<len-1; i++) {
        for(int j=0; j<len-1; j++) {
            if(forward[i] == backward[j+1] && forward[i+1] == backward[j]) {
                cnt++;
            }
        }
    }
    cout << (1<<cnt) << "\n";
}