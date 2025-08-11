#include <iostream>
#include <vector>
#include <set>
using namespace std;
int main() {
  int N, A, B; cin>>N>>A>>B; A--;B--;
  int offset[300];
  for(int i=0; i<N; i++) {
    cin >> offset[i];
  }
  set<int> avalible;
  avalible.insert(A);
  int cnt=0;
  while(avalible.find(B) == avalible.end() && !avalible.empty()) {
    set<int> newAvalible;
    for(const int a : avalible) {
      const int n = offset[a];
      if(a-n > 0) {
        newAvalible.insert(a-n);
      }
      if(a+n <= N) {
        newAvalible.insert(a+n);
      }
    }
    avalible = newAvalible;
    cnt++;
  }
  cout << cnt;
  return 0;
}