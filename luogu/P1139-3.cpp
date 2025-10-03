#include <iostream>
#include <vector>
using namespace std;

/**
#define debug printf
/*/
#define debug(...) 0
/**/

struct TRANS {
  int from, to, train;
};
int n, train[15]={0}, order[15], _time[15];
vector<TRANS> list, result;
vector<int> stack[2];
void log() {
  for(int i=0; i<n; i++) {
    debug("%d ", train[i]);
  }
  debug("| ");
  for(const auto &t : stack[0]) debug("%d ", t);
  debug("| ");
  for(const auto &t : stack[1]) debug("%d ", t);
  debug("\n");
}
void move(int a, int to, int _from=-1) {
  const int from = train[a];
  if(_from>=0 && from!=_from) {
    debug("ERROR: move %d to %d and expect that from %d, but actrually from %d\n", a, to, _from, from);
  }
  // if(from > to) {
  //   debug("ERROR: move %d from %d to %d, invalid move\n", a, from, to);
  // }
  list.push_back({from, to, a});
  if(1<=from && from<=2) {
    if(stack[from-1].empty() || stack[from-1].back()!=a) {
      debug("ERROR: move %d from %d, but top is %d\n", a, from, stack[from-1].empty()?-1:stack[from-1].back());
    }
    stack[from-1].pop_back();
    // debug("pop %d from stack %d\n", a, from-1);
  }
  if(1<=to && to<=2) {
    stack[to-1].push_back(a);
    // debug("push %d to stack %d\n", a, to-1);
  }
  train[a] = to;
  debug("move %d from %d to %d\n", a, from, to);
  log();
}
void undo() {
  // if(list.empty()) throw "error";
  // if(list.empty()) {
  //   debug("ERROR: undo but list is empty\n");
  //   return;
  // }
  const auto &last = list.back();
  train[last.train] = last.from;
  if(1<=last.to && last.to<=2) {
    if(stack[last.to-1].empty() || stack[last.to-1].back()!=last.train) {
      debug("ERROR: undo %d to %d, but top is %d\n", last.train, last.to, stack[last.to-1].empty()?-1:stack[last.to-1].back());
    }
    stack[last.to-1].pop_back();
    // debug("pop %d from stack %d\n", last.train, last.to-1);
  }
  if(1<=last.from && last.from<=2) {
    stack[last.from-1].push_back(last.train);
    // debug("push %d to stack %d\n", last.train, last.from-1);
  }
  debug("undo: move %d from %d to %d\n", last.train, last.from, last.to);
  log();
  list.pop_back();
}
int _result_count=0;
void solve(const int top=0, const int pos=0, int depth=0) {
  if(list.size()-top > 2*n) return;
  if(list.size() != depth) {
    debug("ERROR: depth is %d but list size %d\n", depth, (int)list.size());
    // for(const auto &t : list) {
    //   debug("  %d: %d -> %d\n", t.train+1, t.from, t.to);
    // }
  }
  if(pos == n) {
    debug("result#%d:\n", ++_result_count);
    for(const auto &t : list) {
      debug("  %d: %d -> %d\n", t.train+1, t.from, t.to);
    }
    if(result.empty() || list.size()<result.size()) {
      result = list;
    }
    return;
  }
  // depth++;
  // if(depth > 3*n) return 0;
  if(list.size() > 3*n) return;
  const int next = order[pos];
  const int lastTarg = list.empty()?-1:list.back().to;
  if(top < n) {
    if(next == top) {
      move(next, 3, 0); solve(top+1, pos+1, depth); undo();
    } else {
      move(top, 1, 0); solve(top+1, pos, depth); undo();
      // if(stack[1].empty() || _time[top]<_time[stack[1].back()])
      {
        move(top, 2, 0); solve(top+1, pos, depth); undo();
      }
    }
  }
  if(lastTarg!=1 && !stack[0].empty()) {
    const int _top = stack[0].back();
    if(next == _top) {
      move(_top, 3, 1); solve(top, pos+1, depth); undo();
    } else {
      // if(stack[1].empty() || _time[_top]<_time[next])
      {
        move(_top, 2, 1); solve(top, pos, depth); undo();
      }
    }
  }
  if(lastTarg!=2 && !stack[1].empty()) {
    const int _top = stack[1].back();
    if(next == _top) {
      move(_top, 3, 2); solve(top, pos+1, depth); undo();
    }
  }
}

int main() {
  cin >> n;
  for(int i=0; i<n; i++) {
    char c; cin >> c;
    order[n-1-i] = (n-1) - (c-'a');
    _time[order[n-1-i]] = i;
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