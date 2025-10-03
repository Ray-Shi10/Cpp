#include <iostream>
#include <queue>
#include <algorithm>
using namespace std;

constexpr int E7 = 1e7;
/**/#define debug printf
/*/ #define debug(...) /**/

struct LINE {
  int x1, x2;
  // bool bottom;
  bool operator<(const LINE &b) const {
    return x1 == b.x1 ? x2 > b.x2 : x1 > b.x1;
  }
};
struct TRI {
  int x, y, m;
};

int main() {
  // freopen("data.in", "r", stdin);
  int n;
  cin >> n;
  debug("n = %d\n", n);
  int yCoords[n*2];
  TRI shapes[n];
  int lastY = E7;
  for(int i=0; i<n; i++) {
    int x, y, m;
    cin >> x >> y >> m;
    yCoords[i*2] = y; yCoords[i*2+1] = y+m;
    shapes[i] = {x, y, m};
    lastY = min(lastY, y+m);
  }
  sort(yCoords, yCoords+n*2);
  auto const yCoordsEnd = unique(yCoords, yCoords+n*2);
  for(int i=0; i<n*2; i++) {
    debug("yCoords[%02d] = %d\n", i, yCoords[i]);
    if(yCoords[i+1]<=yCoords[i]) break;
  }
  sort(shapes, shapes+n, [](const TRI &a, const TRI &b) {
    return a.y < b.y;
  });
  priority_queue<LINE> lines[2*n];
  {
    int *p1, *p2; p1 = p2 = yCoords;
    for(auto &shape : shapes) {
      p1 = lower_bound(p1, yCoordsEnd, shape.y);
      p2 = lower_bound(yCoords, yCoordsEnd, shape.y+shape.m);
      debug("shape(%d,%d,%d) -> y[%d~%d] (%d~%d)\n", shape.x, shape.y, shape.m, p1-yCoords+1, p2-yCoords, yCoords[p1-yCoords+1], yCoords[p2-yCoords]);
      for(int y=p1-yCoords+1; y<=p2-yCoords; y++) {
        lines[y].push({shape.x, shape.x+shape.m-(yCoords[y]-shape.y)});
      }
    }
  }
  long long ans = 0;
  // int length[2*n] = {0};
  for(auto &line : lines) {
    LINE last; last.x1 = last.x2 = -E7-1;
    const int y = &line-lines;
    const int h = yCoords[y]-lastY;
    line.empty() || debug("y = %d:\n  h=%d\n", yCoords[y], h);
    while(!line.empty()) {
      LINE now = line.top();
      debug("  pop(%d, %d)\n", now.x1, now.x2);
      line.pop();
      if(now.x1 <= last.x2) last.x2 = max(last.x2, now.x2);
      else {
        const long long add = last.x1<-E7 ? 0 : (((last.x2-last.x1)<<1) + h)*h;
        last.x1<-E7 || debug("  %d ~ %d (%lld+%lld=%lld)\n", last.x1, last.x2, ans, add, ans+add);
        ans += add;
        if(now.x1-last.x2<h && now.x1!=last.x2) {
          const int a = h - (now.x1-last.x2);
          ans -= a*a;
        }
        last = now;
      }
    }
    const long long add = last.x1<-E7 ? 0 : (((last.x2-last.x1)<<1) + h)*h;
    last.x1<-E7 || debug("  %d ~ %d (%lld+%lld=%lld)\n", last.x1, last.x2, ans, add, ans+add);
    ans += add;
    lastY = yCoords[y];
  }
  printf("%.1f\n", ans/2.0);
  return 0;
}