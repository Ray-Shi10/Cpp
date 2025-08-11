#include <iostream>
#include <algorithm>
using namespace std;

class Set {
public:
    int fa[20005] = {0};
    int depth[20005] = {0};
    int find(int x) {
        return (fa[x]==x||fa[x]==0) ? x : find(fa[x]);
    }
    bool comp(int x, int y) {
        return find(x)==find(y) || (fa[x]=y,depth[x]=depth[y]+1,0);
    }
    bool same(int x, int y) {
        return find(x)==find(y) && !(depth[x]+depth[y])&1;
    }
};

struct Relation {
    int a, b;
    int c;
};

int main() {
    int n; //犯罪数
    int m; //关系数
    cin >> n >> m;
    Relation relations[m];
    for(int i=0; i<m; i++) {
        cin >> relations[i].a >> relations[i].b >> relations[i].c;
    }
    sort(relations, relations+m, [](Relation a,Relation b) {
        return a.c > b.c;
    });
    Set set;
    for(int i=0; i<m; i++) {
        for(int j=1; j<=n; j++) {
            printf("%d ", set.fa[j]);
        }
        printf("\n");
        for(int j=1; j<=n; j++) {
            printf("%d ", set.depth[j]);
        }
        printf("\n");
        printf("%d %d\n", relations[i].a, relations[i].b);
        if(set.same(relations[i].a, relations[i].b)) {
            printf("[[ans]]  ");
            cout << relations[i].c << "\n";
            system("pause");
            return 0;
        } else {
            set.comp(set.find(relations[i].a), set.find(relations[i].b));
        }
    }
    cout << 0 << "\n";
    system("pause");
    return 0;
}