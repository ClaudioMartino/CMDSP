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

test_fft: test_fft.o
	$(CXX) test_fft.o -o $@

filter: filter_and_scale.o
	$(CXX) filter_and_scale.o -o $@

modulation: modulation.o
	$(CXX) modulation.o -o $@


test.o: $(TESTING_DIR)/test.cpp complex.hpp $(COMMON_DIR)/assert.hpp $(COMMON_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(TESTING_DIR)/test.cpp

fft.o: $(EXAMPLE_DIR)/fft.cpp complex.hpp $(EXAMPLE_DIR)/fft.hpp $(EXAMPLE_DIR)/wav.hpp $(COMMON_DIR)/assert.hpp $(COMMON_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(EXAMPLE_DIR)/fft.cpp

test_fft.o: $(EXAMPLE_DIR)/test_fft.cpp $(EXAMPLE_DIR)/fft.cpp complex.hpp $(EXAMPLE_DIR)/fft.hpp $(COMMON_DIR)/assert.hpp $(COMMON_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(EXAMPLE_DIR)/test_fft.cpp

filter_and_scale.o: $(EXAMPLE_DIR)/filter_and_scale.cpp complex.hpp $(EXAMPLE_DIR)/fft.hpp $(EXAMPLE_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(EXAMPLE_DIR)/filter_and_scale.cpp

modulation.o: $(EXAMPLE_DIR)/modulation.cpp complex.hpp $(EXAMPLE_DIR)/fft.hpp $(EXAMPLE_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(COMPLEX_DIR) -I$(COMMON_DIR) -c $(EXAMPLE_DIR)/modulation.cpp


clean:
	rm -f *.o
	rm -f test fft test_fft filter modulation
