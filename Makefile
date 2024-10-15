# (c) 2024 Claudio Martino

INC_DIR = Include
TES_DIR = Testing
EXA_DIR = Examples

CXXFLAGS = -std=c++20


fft: fft.o window.o
	$(CXX) fft.o window.o -o $@

filter: filter_and_scale.o
	$(CXX) filter_and_scale.o -o $@

modulation: modulation.o
	$(CXX) modulation.o -o $@

spectrogram: spectrogram.o
	$(CXX) spectrogram.o -o $@

test: test.o
	$(CXX) test.o -o $@

test_fft: test_fft.o
	$(CXX) test_fft.o -o $@


fft.o: $(EXA_DIR)/fft.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/window.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/fft.cpp

window.o: $(EXA_DIR)/window.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/window.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/window.cpp

filter_and_scale.o: $(EXA_DIR)/filter_and_scale.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/filter_and_scale.cpp

modulation.o: $(EXA_DIR)/modulation.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/wav.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/modulation.cpp

spectrogram.o: $(EXA_DIR)/spectrogram.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(EXA_DIR)/spectrogram.cpp

test.o: $(TES_DIR)/test.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(TES_DIR)/test.cpp

test_fft.o: $(TES_DIR)/test_fft.cpp $(EXA_DIR)/fft.cpp $(INC_DIR)/complex.hpp $(INC_DIR)/fft.hpp $(INC_DIR)/assert.hpp $(INC_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $(TES_DIR)/test_fft.cpp


clean:
	rm -f *.o
	rm -f fft window filter modulation spectrogram
	rm -f test test_fft
