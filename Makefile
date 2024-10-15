# (c) 2024 Claudio Martino

SRC_DIR = src
INC_DIR = inc
TES_DIR = test
EXA_DIR = examples

CXXFLAGS = -std=c++20

# Linker
fft_example: fft_example.o wav.o window.o
	$(CXX) fft_example.o wav.o window.o -o $@

filter_example: filter_example.o wav.o
	$(CXX) filter_example.o wav.o -o $@

modulation_example: modulation_example.o wav.o
	$(CXX) modulation_example.o wav.o -o $@

spectrogram_example: spectrogram_example.o wav.o
	$(CXX) spectrogram_example.o wav.o -o $@

test_complex: test_complex.o
	$(CXX) test_complex.o -o $@

test_fft: test_fft.o
	$(CXX) test_fft.o -o $@

# Sources
window.o: $(SRC_DIR)/window.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/window.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(SRC_DIR)/window.cpp

wav.o: $(SRC_DIR)/wav.cpp $(INC_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(SRC_DIR)/wav.cpp

# Examples
fft_example.o: $(EXA_DIR)/fft_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/window.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/fft_example.cpp

filter_example.o: $(EXA_DIR)/filter_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/filter_example.cpp

modulation_example.o: $(EXA_DIR)/modulation_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/modulation_example.cpp

spectrogram_example.o: $(EXA_DIR)/spectrogram_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/spectrogram_example.cpp

# Tests
test_complex.o: $(TES_DIR)/test_complex.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(TES_DIR)/test_complex.cpp

test_fft.o: $(TES_DIR)/test_fft.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(TES_DIR)/test_fft.cpp


clean:
	rm -f *.o
	rm -f fft_example filter_example modulation_example spectrogram_example
	rm -f test_complex test_fft
