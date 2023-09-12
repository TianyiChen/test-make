#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
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
auto get_timespec_local_day() {
  timespec rt;
  timespec_get(&rt, TIME_UTC);
  rt.tv_sec -= 5 * 3600;
  rt.tv_sec %= 3600;
  return rt;
}
void format1(ofstream& o) {
  auto t = get_timespec_local_day();
  char timeString[std::size("yyyy-mm-dd hh:mm:ss")];
  std::strftime(std::data(timeString), std::size(timeString), "%F %T", localtime(&t.tv_sec));
  o << timeString << '.' << std::setfill('0') << std::setw(9) << t.tv_nsec;
}
void pad2(ofstream& o, int num) {
  if(num < 10) o << '0';
  o << num;
}
void format2(ofstream& o) {
  auto t = get_timespec_local_day();
  tm   l;
  localtime_r(&t.tv_sec, &l);
  o << (l.tm_year + 1900) << '-';
  if(l.tm_mon < 9) o << '0';
  o << l.tm_mon + 1 << '-';
  pad2(o, l.tm_mday);
  o << ' ';
  pad2(o, l.tm_hour);
  o << ':';
  pad2(o, l.tm_min);
  o << ':';
  pad2(o, l.tm_sec);
  o << '.' << std::setfill('0') << std::setw(9) << t.tv_nsec;
}
int main() {
  int      cnt1 = 0, cnt2 = 0;
  ofstream o("/dev/null");
  {
    auto start = chrono::system_clock::now();
    auto end   = start - chrono::floor<chrono::days>(start) + 1s;
    for(;;) {
      auto now = chrono::system_clock::now();
      if(now - chrono::floor<chrono::days>(now) >= end) break;
      format1(o);
      ++cnt1;
    }
  }
  {
    auto start = get_timespec_local_day();
    auto end   = start;
    end.tv_sec += 1;
    for(;;) {
      auto now = get_timespec_local_day();
      if(now >= end) break;
      format2(o);
      ++cnt2;
    }
  }
  cout << cnt1 << ' ' << cnt2 << endl;
}
