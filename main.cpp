#include <iostream>
#include <ctime>
#include <cstdlib>
#include "tane.h"


int main(int argc, char **argv) {
  if (argc < 4) {
	  std::cout << "ERROR: Not enough arguments\nUSAGE: ./tane input_file output_file attr_num\n";
    
    return 1;
  }

  clock_t start = clock();

  Tane tane(argv[1], atoi(argv[3]));
  tane.ComputeDependencies();
  tane.Output(argv[2]);

  clock_t end = clock();
  std::cout << "Runtime: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

  return 0;
}