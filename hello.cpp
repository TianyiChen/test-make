#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;
struct TS {
  timespec v;
  static_assert(is_signed_v<decltype(v.tv_nsec)>);
  static TS now() {
    timespec r;
    clock_gettime(CLOCK_REALTIME, &r);
    return {r};
  }
  TS operator-(const TS& o) const {
    TS rt{v.tv_sec - o.v.tv_sec, v.tv_nsec - o.v.tv_nsec};
    if(rt.v.tv_nsec < 0) rt.v.tv_nsec += 1e9, --rt.v.tv_sec;
    return rt;
  }
  auto operator<=>(const TS& o) const {
    return v.tv_sec != o.v.tv_sec ? v.tv_sec <=> o.v.tv_sec : v.tv_nsec <=> o.v.tv_nsec;
  }
};
int main() {
  int  cnt1 = 0, cnt2 = 0;
  auto start = chrono::system_clock::now();
  for(;;) {
    auto now = chrono::system_clock::now();
    if(now - start >= 1s) break;
    ++cnt1;
  }
  auto start2 = TS::now();
  for(;;) {
    auto now = TS::now();
    if(now - start2 >= TS{1, 0}) break;
    ++cnt2;
  }
  cout << cnt1 << ' ' << cnt2 << endl;
}
