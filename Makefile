CPP= $(CXX) -std=c++20 -O2
test:
	$(CPP) hello.cpp -o hello
	./hello
