#include <iostream>
using namespace std;
#define printf(...)
int main() {
    int n, m;
    cin >> n >> m;
    bool _bigger = n>m;
    if(_bigger) swap(n, m);
    int a[n][m];
    srand(time(0));
    if(_bigger) {
        for(int i=0; i<m; i++) {
            for(int j=0; j<n; j++) {
                cin >> a[j][i];
                // a[j][i] = rand()%10;
            }
        }
    } else {
        for(int i=0; i<n; i++) {
            for(int j=0; j<m; j++) {
                cin >> a[i][j];
                // a[i][j] = rand()%10;
                printf("%d ", a[i][j]);
            }
            printf("\n");
        }
    }
    int dp[n+m][n][n];
    for(int i=0; i<n+m; i++) {
        for(int j=0; j<n; j++) {
            for(int k=0; k<n; k++) {
                dp[i][j][k] = -1e9;
            }
        }
    }
    dp[1][n-2][n-1] = a[0][1] + a[1][0];
    for(int sum=2; sum<n+m-2; sum++) {
        const int maxInd = min(m+n-2-sum, n-1);
        const int minInd = max(0, n-1-sum);
        for(int i=minInd; i<=maxInd; i++) {
            for(int j=i+1; j<=maxInd; j++) {
                dp[sum][i][j] = max(dp[sum-1][i][j], dp[sum][i][j]);
                if(j<n-1) dp[sum][i][j] = max(dp[sum-1][i][j+1], dp[sum][i][j]);
                if(i<n-1) dp[sum][i][j] = max(dp[sum-1][i+1][j], dp[sum][i][j]);
                if(i<n-1&&j<n-1) dp[sum][i][j] = max(dp[sum-1][i+1][j+1], dp[sum][i][j]);
                // if(j) dp[sum][i][j] = max(dp[sum-1][i][j-1], dp[sum][i][j]);
                // if(i) dp[sum][i][j] = max(dp[sum-1][i-1][j], dp[sum][i][j]);
                // if(i&&j) dp[sum][i][j] = max(dp[sum-1][i-1][j-1], dp[sum][i][j]);
                printf("dp[%d][%d][%d] = %d", sum, i, j, dp[sum][i][j]);
                dp[sum][i][j] += a[n-i-1][sum-n+i+1] + a[n-j-1][sum-n+j+1];
                printf(" + (a[%d][%d]=%d) + (a[%d][%d]=%d) = %d\n", sum-n+i+1, n-i-1, a[n-i-1][sum-n+i+1], sum-n+j+1, n-j-1, a[n-j-1][sum-n+j+1], dp[sum][i][j]);
            }
        }
    }
    cout << dp[n+m-3][0][1];
}