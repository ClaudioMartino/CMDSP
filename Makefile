# 2024 Claudio Martino

CXXFLAGS = -std=c++20

main: main.o
	$(CXX) main.o -o $@

main.o: main.cpp complex.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

run:
	./main

clean:
	rm -f main.o
	rm main
