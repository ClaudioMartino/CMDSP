# (c) 2024 Claudio Martino

CPX_DIR = .
TES_DIR = Testing
EXA_DIR = Examples
COM_DIR = Common

CXXFLAGS = -std=c++20


test: test.o
	$(CXX) test.o -o $@

fft: fft.o window.o
	$(CXX) fft.o window.o -o $@

test_fft: test_fft.o
	$(CXX) test_fft.o -o $@

filter: filter_and_scale.o
	$(CXX) filter_and_scale.o -o $@

modulation: modulation.o
	$(CXX) modulation.o -o $@

spectrogram: spectrogram.o
	$(CXX) spectrogram.o -o $@


test.o: $(TES_DIR)/test.cpp complex.hpp $(COM_DIR)/assert.hpp $(COM_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(TES_DIR)/test.cpp

fft.o: $(EXA_DIR)/fft.cpp complex.hpp $(EXA_DIR)/fft.hpp $(EXA_DIR)/wav.hpp $(EXA_DIR)/window.hpp $(COM_DIR)/assert.hpp $(COM_DIR)/random.hpp $(COM_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(EXA_DIR)/fft.cpp

test_fft.o: $(EXA_DIR)/test_fft.cpp $(EXA_DIR)/fft.cpp complex.hpp $(EXA_DIR)/fft.hpp $(COM_DIR)/assert.hpp $(COM_DIR)/random.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(EXA_DIR)/test_fft.cpp

filter_and_scale.o: $(EXA_DIR)/filter_and_scale.cpp complex.hpp $(EXA_DIR)/fft.hpp $(EXA_DIR)/wav.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(EXA_DIR)/filter_and_scale.cpp

modulation.o: $(EXA_DIR)/modulation.cpp complex.hpp $(EXA_DIR)/fft.hpp $(EXA_DIR)/wav.hpp $(COM_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(EXA_DIR)/modulation.cpp

window.o: $(EXA_DIR)/window.cpp complex.hpp $(EXA_DIR)/window.hpp $(COM_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(EXA_DIR)/window.cpp

spectrogram.o: $(EXA_DIR)/spectrogram.cpp complex.hpp $(COM_DIR)/constants.hpp
	$(CXX) $(CXXFLAGS) -I$(CPX_DIR) -I$(COM_DIR) -c $(EXA_DIR)/spectrogram.cpp


clean:
	rm -f *.o
	rm -f test fft test_fft filter modulation window spectrogram
