# (c) 2024 Claudio Martino

INC_DIR = inc
TES_DIR = test
EXA_DIR = examples

CXXFLAGS = -std=c++20

EXAMPLES = fft_example filter_example modulation_example spectrogram_example walsh_example
TESTS = test_complex test_fft

all: $(EXAMPLES) $(TESTS)

examples: $(EXAMPLES)

tests: $(TESTS)

# Linker
fft_example: fft_example.o
	$(CXX) $< -o $@

filter_example: filter_example.o
	$(CXX) $< -o $@

modulation_example: modulation_example.o
	$(CXX) $< -o $@

spectrogram_example: spectrogram_example.o
	$(CXX) $< -o $@

walsh_example: walsh_example.o
	$(CXX) $< -o $@

test_complex: test_complex.o
	$(CXX) $< -o $@

test_fft: test_fft.o
	$(CXX) $< -o $@

# Examples
fft_example.o: $(EXA_DIR)/fft_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/window.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

filter_example.o: $(EXA_DIR)/filter_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

modulation_example.o: $(EXA_DIR)/modulation_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

spectrogram_example.o: $(EXA_DIR)/spectrogram_example.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

walsh_example.o: $(EXA_DIR)/walsh_example.cpp $(INC_DIR)/walsh_hadamard.hpp $(INC_DIR)/matrix.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

# Tests
test_complex.o: $(TES_DIR)/test_complex.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

test_fft.o: $(TES_DIR)/test_fft.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $<

check:
	./test_complex
	./test_fft

clean:
	rm -f *.o
	rm -f $(EXAMPLES)
	rm -f $(TESTS)
