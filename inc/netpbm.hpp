#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>

uint8_t rgb_to_grayscale(uint8_t red, uint8_t green, uint8_t blue) {
  return round(0.299 * red + 0.587 * green + 0.114 * blue);
}

struct netpbm {
  bool is_baw() {
    return (magic_number == "P1" or magic_number == "P4");
  }

  bool is_gray() {
    return (magic_number == "P2" or magic_number == "P5");
  }

  bool is_rgb() {
    return (magic_number == "P3" or magic_number == "P6");
  }

  void get_size() {
    if(this->is_rgb())
      size = 3*width*height;
    else
      size = width*height;
  }

  void decoder(std::string filename) {
    std::ifstream fsi(filename, std::ios::binary);
    if(fsi.is_open()) {
      std::string width_s, height_s, max_s;
  
      // First line
      getline(fsi, magic_number);
  
      // Second line
      getline(fsi, width_s, ' ');
      width = stoi(width_s);
      getline(fsi, height_s);
      height = stoi(height_s);
  
      get_size();

      if(magic_number != "P1" && magic_number != "P4") {
        // Third line
        getline(fsi, max_s);
        max = stoi(max_s);
        if(this->is_rgb()) {
          if(max != 255) {
            std::cout << "err" << std::endl;
            exit(1);
          }
        }
      }

      // Rest of the file
      pixels = new char[size];
      fsi.read(pixels, size*sizeof(char));
  
      fsi.close();
    }
    else {
      std::cout << "Cannot open " << filename << std::endl;
      exit(1);
    }
  }

  void encoder(std::string filename_, std::string ext) {
    std::string filename = filename_ + "." + ext;
    std::ofstream fso(filename, std::ios::binary);

    if(fso.is_open()) {
      // Save RGB file
      if(ext == "ppm") {
        // From B&W
        if (this->is_baw()) {
          std::cout << "Can't convert pbm to ppm." << std::endl;
          exit(1);
        }
        // From Grayscale
        if (this->is_gray()) {
          std::cout << "Can't convert pgm to ppm." << std::endl;
          exit(1);
        }
        // From RGB
        fso << "P6" << std::endl;
        fso << width << " " << height << std::endl;
        fso << max << std::endl;
        for(size_t i=0; i<size; i++)
          fso << pixels[i];
      }

      // Save grayscale file
      if(ext == "pgm") {
        // From B&W
        if (this->is_baw()) {
          std::cout << "Can't convert pbm to pgm." << std::endl;
          exit(1);
        }

        fso << "P5" << std::endl;
        fso << width << " " << height << std::endl;
        fso << max << std::endl;
        // From RGB
        if (this->is_rgb()) {
          for(size_t i=0; i<size/3; i++)
            fso << rgb_to_grayscale((uint8_t)pixels[3*i], (uint8_t)pixels[3*i + 1], (uint8_t)pixels[3*i + 2]);
        }
        // From grayscale
        if (this->is_gray()) {
          for(size_t i=0; i<size; i++)
            fso << pixels[i];
        }
      }

      // Save black and white file
      if(ext == "pbm") {
        fso << "P4" << std::endl;
        fso << width << " " << height << std::endl;
        // From RGB
        if (this->is_rgb()) {
          for(size_t i=0; i<size/3 >> 3; i++) {
            uint8_t tmp = 0b00000000;
            for(size_t j=0; j<8; j++) {
              if(rgb_to_grayscale((uint8_t)pixels[3*(8*i + j)], (uint8_t)pixels[3*(8*i + j) + 1], (uint8_t)pixels[3*(8*i + j) + 2]) < 128) {
                tmp |= (0b10000000 >> j);
              }
            }
            fso << tmp;
          }
        }
        // From grayscale
        if (this->is_gray()) {
          for(size_t i=0; i<size >> 3; i++) {
            uint8_t tmp = 0b00000000;
            for(size_t j=0; j<8; j++) {
              if((uint8_t)pixels[8*i + j] < 128) {
                tmp |= (0b10000000 >> j);
              }
            }
            fso << tmp;
          }
        }
        // From B&W
        if (this->is_baw()) {
          for(size_t i=0; i<size >> 3; i++) {
            fso << pixels[i];
          }
        }
      }
  
      fso.close();
    }
    else {
      std::cout << "Cannot open " << filename << std::endl;
      exit(1);
    }
  }

  private:
    std::string magic_number;
    int width, height, max;
    char * pixels;
    size_t size;
};


