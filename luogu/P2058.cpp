#include <iostream>
#include <queue>
using namespace std;

#define check(a) a
#define log(val) {printf(#val" = %d\n", val);}
#define printf(...)

struct Ship {
    int time;
    int k;
    bool country[10005] = {0};
};

/*template <typename T>
class queue {
public:
    T *data;
    T *st, *ed;
    // size_t size=0;
    size_t totSize;
    queue(size_t size) : totSize(size+5) {
        st = ed = data = new T[size+5];
    }
    T &front() {
        check(if(empty()) throw "pop empty queue";)
        return *data;
    }
    T front() const {
        check(if(empty()) throw "pop empty queue";)
        return *data;
    }
    T *push(const T &val) {
        *ed = val;
        T *ans = ed;
        ed++;
        if(ed-data >= totSize) {
            ed = data;
        }
        return ans;
    }
    bool empty() {
        return st>=ed;
    }
    T pop() {
        check(if(empty()) throw "pop empty queue";)
        T ans = *st;
        st++;
        if(st-data >= totSize) {
            st = data;
        }
        return ans;
    }
};
*/
int main() {
    int n; cin >> n;
    deque<Ship> information(n);
    for(int i=0; i<n; i++) {
        Ship ship;
        log(i);
        // printf("000000000000000\n");
        cin >> ship.time;
        cin >> ship.k;
        for(int j=0; j<ship.k; j++) {
            int country;
            cin >> country;
            ship.country[country] = true;
        }
        // printf("1\n");
        information.push_back(ship);
        while(!information.empty() && information.front().time <= ship.time-86400) {
            information.pop_front();
        }
        // printf("2\n");
        bool country[10005] = {0};
        for(Ship &info : information) {
            for(int i=0; i<10005; i++) {
                country[i] |= info.country[i];
            }
        }
        for(int i=0; i<10; i++)
            printf("%d ", country[i]);
        printf("\n");
        int cnt=0;
        for(int i=0; i<10005; i++) {
            if(country[i])
                cnt++;
        }
        cout << cnt << "\n";
    }
    system("pause");
    return 0;
}