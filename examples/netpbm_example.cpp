#include <getopt.h>

#include "netpbm.hpp"

int main(int argc, char** argv) {
  // Default value
  std::string filename = "examples/edwige_256.ppm";

  // Read options
  for(;;) {
    switch(getopt(argc, argv, "n:r:f:sbhw")) {
      case 'f':
        filename = optarg;
        continue;
      case 'h':
      default :
        printf("Usage: netpbm_example [-f file.ppm]\n");
        return 0;
        break;
      case -1:
        break;
    }
    break;
  }

  netpbm n;

  // Open .ppm RGB image
  n.decoder(filename);
  std::string filename_clean = filename.substr(0, filename.size() - 4);
  std::string filename_ppm = filename_clean + "_from_ppm";
  // Save .ppm RGB image (again)
  n.encoder(filename_ppm, "ppm");
  // Save .pgm grayscale image
  n.encoder(filename_ppm, "pgm");
  // Save .pgm grayscale image
  n.encoder(filename_ppm, "pbm");

  // Open .pgm grayscale image
  n.decoder(filename_ppm + ".pgm");
  std::string filename_ppm_clean = filename_ppm.substr(0, filename.size() - 4);
  std::string filename_pgm = filename_ppm_clean + "_from_pgm";
  // Save .pgm grayscale image (again)
  n.encoder(filename_pgm, "pgm");
  // Save .pgm grayscale image
  n.encoder(filename_pgm, "pbm");

  // Open .pbm black and white image
  n.decoder(filename_ppm + ".pbm");
  std::string filename_pbm = filename_ppm_clean + "_from_pbm";
  // Save .pbm grayscale image (again)
  n.encoder(filename_pbm, "pbm");

  return 0;
}
