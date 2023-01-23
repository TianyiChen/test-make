#include<bits/stdc++.h>
#include"json.hpp"
using namespace std;
template <class T>
struct TestAlloc {
	T *allocate(size_t n) {
		return new T[n];
	}
	void deallocate(T *p, std::size_t n) {
		std::cerr << "delete " << (void *)p << ' ' << n << '\n';
		delete[] p;
	}
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
};
template <class T>
using Alloc = TestAlloc<T>;
int main(){
	using J = nlohmann::basic_json<std::map, std::vector, basic_string<char, char_traits<char>, Alloc<char>>, bool, int64_t, uint64_t, double, Alloc>;
	auto j = J ::parse(R"(
	 {
    "happy": true,
    "pi": 3.141,
	"hello": "world"
 	 }
	)");
	cout<<j["hello"]<<endl;
}
