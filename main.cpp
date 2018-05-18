#include <iostream>
#include <ctime>
#include "tane.h"


int main(int argc, char **argv) {
  if (argc < 3) {
	  std::cout << "ERROR: Not enough arguments" << std::endl;
    return 1;
  }

  clock_t start = clock();

  Tane tane(argv[1], 15);
  tane.ComputeDependencies();
  tane.Output(argv[2]);

  clock_t end = clock();
  std::cout << "Runtime: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

  return 0;
}