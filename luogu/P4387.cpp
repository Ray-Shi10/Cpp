#include <iostream>
//#include <stack>
using namespace std;

#define printf(...)

#define log(val) {printf(#val);printf(": %d\n", val);}
void print(int n, int a[]) {
    printf("%d:", n);
    for(int i=0; i<n; i++) {
        printf(" %d", a[i]);
    }
    printf("\n");
}

bool solve(int n, int pop[]) {
    print(n, pop);
    int stk[n], len=0, progress=0;
    for(int i=0; i<n; i++) {
        log(i);
        int next = pop[i];
        int now = len?stk[len-1]:-1;
        print(len, stk);
        log(next);
        log(now);
        log(progress);
        if(next >= progress) {
            while(progress <= next) {
                stk[len] = progress;
                printf("push(%d)\n", stk[len]);
                len++; progress++;
            }
            //progress++;
            len--;
            printf("pop(%d)\n", stk[len]);
        } else if(next == now) {
            len--;
            printf("pop(%d)\n", stk[len]);
        } else /*error*/ {
            printf("fail.\n");
            return false;
        }
    }
    return true;
}

int main() {
    //freopen(".in", "r", stdin);
    int q;
    cin >> q;
    for(int i=0; i<q; i++) {
        int n;
        cin >> n;
        int a[n], b[n];
        for(int j=0; j<n; j++) {
            int num;
            cin >> num;
            a[num-1] = j;
        }
        for(int j=0; j<n; j++) {
            int num;
            cin >> num;
            b[j] = a[num-1];
        }
        cout << (solve(n, b)?"Yes":"No") << "\n";
    }
    system("pause");
}