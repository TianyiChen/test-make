CPP= $(CXX) -std=c++20 -O2 -flto -march=native
ifneq (,$(findstring clang,$(CXX)))
CPP := $(CPP) -stdlib=libc++
endif
test:
	$(CPP) hello.cpp -o hello -lstdc++
