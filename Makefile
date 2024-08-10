# (c) 2024 Claudio Martino

COMPLEX_DIR = .
TESTING_DIR = Testing
EXAMPLE_DIR = Examples
COMMON_DIR = Common

CXXFLAGS = -std=c++20

test: test.o
	$(CXX) test.o -o $@

fft: fft.o
	$(CXX) fft.o -o $@

test.o: $(TESTING_DIR)/test.cpp complex.hpp $(COMMON_DIR)/assert.hpp $(COMMON_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(TESTING_DIR)/test.cpp

fft.o: $(EXAMPLE_DIR)/fft.cpp complex.hpp $(COMMON_DIR)/assert.hpp $(COMMON_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(EXAMPLE_DIR)/fft.cpp

clean:
	rm -f *.o
	rm -f test fft
