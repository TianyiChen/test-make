#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

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
int get_timezone_offset_s() {
  time_t t = time(NULL);
  tm     lt;
  localtime_r(&t, &lt);
  return lt.tm_gmtoff;
}
auto get_timespec_local() {
  timespec rt;
  timespec_get(&rt, TIME_UTC);
  // assumes timezone doesn't change while the program is running
  static int tz_offset = get_timezone_offset_s();
  rt.tv_sec += tz_offset;
  return rt;
}
auto get_timespec_local_day() {
  auto r = get_timespec_local();
  r.tv_sec %= 86400;
  return r;
}
void format1(ostream& o, timespec t = get_timespec_local()) {
  char timeString[std::size("yy-mm-dd hh:mm:ss")];
  std::strftime(std::data(timeString), std::size(timeString), "%y-%m-%d %T", gmtime(&t.tv_sec));
  o << timeString << '.' << std::setfill('0') << std::setw(9) << t.tv_nsec;
}
void format2(ostream& o, timespec t = get_timespec_local()) {
  tm l;
  gmtime_r(&t.tv_sec, &l);
  // 2000-2099
  char       buf[std::size("13-01-02 12:34:56.123456789")], *p = buf;
  const auto pad2 = [&](int num) {
    *p++ = num / 10 + '0';
    *p++ = num % 10 + '0';
  };
  pad2(l.tm_year - 100);
  *p++ = '-';
  pad2(l.tm_mon + 1);
  *p++ = '-';
  pad2(l.tm_mday);
  *p++ = ' ';
  pad2(l.tm_hour);
  *p++ = ':';
  pad2(l.tm_min);
  *p++ = ':';
  pad2(l.tm_sec);
  *p++ = '.';
  for(char* q = p + 8; q >= p; t.tv_nsec /= 10) {
    *q-- = t.tv_nsec % 10 + '0';
  }
  o << string_view{buf, end(buf)};
}
int main() {
  cerr << get_timezone_offset_s() << endl;
  stringstream s;
  format1(s);
  s << '\n';
  format2(s);
  cerr << s.str() << endl;
  int      cnt1 = 0, cnt2 = 0;
  ofstream o("/dev/null");
  {
    auto start = get_timespec_local_day();
    auto end   = start;
    end.tv_sec += 1;
    for(;;) {
      auto now = get_timespec_local_day();
      if(now >= end) break;
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
  cout << cnt1 << ' ' << cnt2 << ' ' << double(cnt2) / cnt1 << endl;
}
