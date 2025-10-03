#include <iostream>
#include <vector>
using namespace std;

/** #define debug printf
/*/ #define debug(...) 0/**/

struct TRANS {
  int from, to, train;
};
int n, train[15]={0}, order[15];
vector<TRANS> list, result;
vector<int> stack[2];
void move(int a, int to, int _from=-1) {
  const int from = train[a];
  if(_from>=0 && from!=_from) {
    debug("ERROR: move %d to %d and expect that from %d, but actrually from %d\n", a, to, _from, from);
  }
  debug("move %d from %d to %d\n", a, from, to);
  list.push_back({from, to, a});
  if(1<=from && from<=2) {
    if(stack[from-1].empty() || stack[from-1].back()!=a) {
      debug("ERROR: move %d from %d, but top is %d\n", a, from, stack[from-1].empty()?-1:stack[from-1].back());
    }
    stack[from-1].pop_back(), debug("pop %d from stack %d\n", a, from-1);
  }
  if(1<=to && to<=2) stack[to-1].push_back(a), debug("push %d to stack %d\n", a, to-1);
  train[a] = to;
}
void undo() {
  // if(list.empty()) throw "error";
  const auto &last = list.back();
  list.pop_back();
  // train[last.train] = last.from;
  // debug("undo: ");
  move(last.train, last.from, last.to);
  list.pop_back();
}
int _result_count=0;
bool solve(const int top=0, const int pos=0) {
  if(pos == n) {
    debug("result#%d:\n", ++_result_count);
    for(const auto &t : list) {
      debug("  %d: %d -> %d\n", t.train+1, t.from, t.to);
    }
    if(result.empty() || list.size()<result.size()) {
      result = list;
    }
    return 1;
  }
  const int next = order[pos];
  if(next > top) {
    move(top, 1, 0);
    if(solve(top+1, pos)) ;
    undo();
    move(top, 2, 0);
    if(solve(top+1, pos)) ;
    undo();
  } else if(next == top) {
    move(next, 3, 0);
    if(solve(top+1, pos+1)) ;
    undo();
  } else {
    if(!stack[train[next]-1].empty() && stack[train[next]-1].back() == next) {
      move(next, 3);
      if(solve(top, pos+1)) ;
      undo();
    } else {
      if(train[next]==1) {
        int cnt = 0;
        while(!stack[0].empty() && stack[0].back()!=next) {
          move(stack[0].back(), 2);
          cnt++;
        }
        move(next, 3);
        if(solve(top, pos+1)) ;
        for(int i=0; i<=cnt; i++) undo();
      }
    }
  }
  return 0;
}

int main() {
  cin >> n;
  for(int i=0; i<n; i++) {
    char c; cin >> c;
    order[n-1-i] = (n-1) - (c-'a');
  }
  solve();
  if(result.empty()) {
    printf("NO\n");
  } else {
    for(const auto &t : result) {
      printf("%c %c %c\n", n-1-t.train+'a', t.from+'A', t.to+'A');
    }
  }
  result.clear();
  list.clear();
  stack[0].clear();
  stack[1].clear();
  for(int i=0; i<15; i++) train[i] = 0;
  main();
  return 0;
}