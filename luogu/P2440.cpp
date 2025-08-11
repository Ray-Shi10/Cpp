#include <iostream>
using namespace std;

#define log(val) printf(#val" = %d\n", val)
// #define printf(...)

void print(int n, int a[]) {
    printf("%d:", n);
    for(int i=0; i<n; i++) {
        printf(" %d", a[i]);
    }
    printf("\n");
}

int main() {
    int n, k;
    cin >> n >> k;
    int L[n];
    int l, min=0, max=1e8+5;
    for(int i=0; i<n; i++) {
        cin >> L[i];
        // if(L[i] > max) max=L[i];
    }
    print(n, L);
    while(max-min > 1) {
        l = (min+max) >> 1;
        log(min);
        log(max);
        log(l);
        int cnt=0;
        for(int i=0; i<n; i++) {
            cnt += L[i]/l;
        }
        log(cnt);
        if(cnt >= k) {
            min = l;
            printf("case1\n");
        } else {
            max = l;
            printf("case2\n");
        }
    }
    cout << min << "\n";
    system("pause");
    return 0;
}