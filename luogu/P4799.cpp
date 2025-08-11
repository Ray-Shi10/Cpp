#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;

#define int long long
// #define printf(...)

signed main() {
    int n; //总场数
    int m; //钱数
    cin >> n;
    cin >> m;
    int a[n];
    for(int i=0; i<n; i++) {
        cin >> a[i];
    }
    printf("\n");
    sort(a, a+n);
    const int mid = n>>1;
    printf("sorted:\n");
    for(int i=0; i<n; i++) {
        printf("%d ", a[i]);
        if(i == mid) printf("| ");
    }
    printf("\n\n");
    map<int, int> state1, state2;
    state1[0] = state2[0] = 1;
    for(int i=n-1; i>mid; i--) {
        printf("new-turn (i:%d)\n", i);
        printf("cost: %d\n", a[i]);
        // printf("keys: ");
        vector<int> keys;
        for(const auto &it : state2) {
            keys.push_back(it.first);
            // printf("%d ", it.first);
        }
        // printf("\n");
        for(int j=keys.size()-1; j>=0; j--) {
            const int key = keys[j];
            printf("try to buy %d\n", key);
            if(key+a[i] <= m) {
                state2[key+a[i]] += state2[key];
            }
        }
        printf("new-state: {\n");
        for(const auto &it : state2) {
            printf("   %d: %d\n", it.first, it.second);
        }
        printf("}\n");
        // printf("now-ans: %d\n", ans);
        printf("\n");
    }
    printf("---state2---\n");
    for(int i=mid; i>=0; i--) {
        printf("turn#%d:\n", i);
        printf("cost: %d\n", a[i]);
        vector<int> keys;
        for(const auto &it : state1) {
            keys.push_back(it.first);
        }
        for(int j=keys.size()-1; j>=0; j--) {
            const int key = keys[j];
            if(key+a[i] <= m) {
                state1[key+a[i]] += state1[key];
            }
        }
        printf("new-state: {\n");
        for(const auto &it : state1) {
            printf("   %d: %d\n", it.first, it.second);
        }
        printf("}\n");
        // printf("now-ans: %d\n", ans);
        printf("\n");
    }
    int ans = 0;
    vector<int> keys1, keys2;
    for(const auto &i : state1) {
        keys1.push_back(i.first);
    }
    for(const auto &i : state2) {
        keys2.push_back(i.first);
    }
    printf("state1: {\n");
    for(const auto &it : state1) {
        printf("   %d: %d\n", it.first, it.second);
    }
    printf("}\n");
    printf("state2: {\n");
    for(const auto &it : state2) {
        printf("   %d: %d\n", it.first, it.second);
    }
    printf("}\n");
    printf("\n");
    int sum = 0;
    for(int i=keys1.size()-1, j=0; i>=0; i--) {
        printf("i:%d, key1:%d\n", i, keys1[i]);
        while(keys1[i] + keys2[j] <= m && j<keys2.size()) {
            printf("j:%d, key2:%d\n", j, keys2[j]);
            sum += state2[keys2[j]];
            j++;
        }
        printf("i:%d, j:%d\n", i, j);
        ans += sum * state1[keys1[i]];
        printf("ans: %d (+%d*%d=%d)\n", ans, sum, state1[keys1[i]], sum*state1[keys1[i]]);
        printf("\n");
    }
    cout << ans << "\n";
    system("pause");
    return 0;
}