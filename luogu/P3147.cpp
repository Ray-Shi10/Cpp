#include <iostream>
using namespace std;

int main() {
    int ans = 0;
    int n;
    cin >> n;
    int a[n];
    int dp1[n]={0}, dp2[n]={0};
    for(int i=0; i<n; i++) {
        cin >> a[i];
    }
    ans = dp1[0] = dp2[0] = a[0];
    for(int i=1; i<n; i++) {
        dp2[i] = dp1[i] = a[i];
        int j=i-1;
        while(j>=0 && dp1[j]<=dp2[i] && dp2[i]<=dp2[j]) {
            dp2[i]++;
            j--;
        }
        ans = max(ans, dp2[i]);
        printf("%d ~ %d\n", dp1[i], dp2[i]);
    }
    cout << ans << "\n";
    system("pause");
    return 0;
}