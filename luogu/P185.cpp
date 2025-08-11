#include <iostream>
using namespace std;

int main() {
    // init
    int m; //二叉树层数
    int n; //删除的点数
    cin >> m >> n;
    bool tree[1<<n] = {true};
    // delete
    for(int i=0; i<n; i++) {
        int x, y;
        cin >> x >> y;
        x--; y--;
        tree[(1<<x)+y-1] = false;
    }
    // print
    int shifts[m+1];
    shifts[0] = 0;
    for(int i=0; i<m; i++) {
        shifts[i+1] = 2 * shifts[i] + 1;
    }
    for(int row=m-1; row>=0; row--) {
        const int shift = shifts[row+1];
        for(int i=0; i<shift; i++) {
            cout << " ";
        }
        for(int i=0; i<(1<<(m-1-row)); i++) {
            cout << tree[1<<(m-1-row)+i-1] ? "o" : " ";
            // for(int i=0; )
        }
    }
    return 0;
}