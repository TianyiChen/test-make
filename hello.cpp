#include <chrono>
#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;
struct TS {
  timespec v;
  static_assert(is_signed_v<decltype(v.tv_nsec)>);
  static TS now() {
    TS r;
    clock_gettime(CLOCK_REALTIME, &r.v);
    return r;
  }
  TS operator-(const TS& o) const {
    TS rt{v.tv_sec - o.v.tv_sec, v.tv_nsec - o.v.tv_nsec};
    if(rt.v.tv_nsec < 0) rt.v.tv_nsec += 1e9, --rt.v.tv_sec;
    return rt;
  }
  auto operator<=>(const TS& o) const {
    return v.tv_sec != o.v.tv_sec ? v.tv_sec <=> o.v.tv_sec : v.tv_nsec <=> o.v.tv_nsec;
  }
  TS& from_midnight() {
    v.tv_sec %= 86400;
    return *this;
  }
};

auto operator<=>(const timespec& a, const timespec& b) {
  return a.tv_sec != b.tv_sec ? a.tv_sec <=> b.tv_sec : a.tv_nsec <=> b.tv_nsec;
}
auto get_timespec() {
  timespec rt;
  timespec_get(&rt, TIME_UTC);
  return rt;
}
time_t midnight_timestamp() {
  auto t = time(0);
  tm   midnight;
  localtime_r(&t, &midnight);
  midnight.tm_hour = 0;
  midnight.tm_min  = 0;
  midnight.tm_sec  = 0;
  return mktime(&midnight);
}
int main() {
  int cnt1 = 0, cnt2 = 0;
  {
    auto start = chrono::system_clock::now();
    auto end   = start - chrono::floor<chrono::days>(start) + 1s;
    for(;;) {
      auto now = chrono::system_clock::now();
      if(now - chrono::floor<chrono::days>(now) >= end) break;
      ++cnt1;
    }
  }
  {
    auto start = get_timespec();
    auto end   = start;
    end.tv_sec -= midnight_timestamp();
    end.tv_sec += 1;
    for(;;) {
      auto now = get_timespec();
      now.tv_sec -= midnight_timestamp();
      if(now >= end) break;
      ++cnt2;
    }
  }
  cout << cnt1 << ' ' << cnt2 << endl;
}
