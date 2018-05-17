#ifndef TANE_H
#define TANE_H

#include <set>
#include "utils.h"


// The collection of attribute sets of a certain size
struct Level {
	int size;
	std::set<int> attr_sets;
};

extern struct Level* GenerateNextLevel(struct Level* level, int attr_num);


class Tane {
 public:
  Tane(const std::string file_name, int attr_num);
  ~Tane();

  void Run();  

 private:
  void ComputeDependencies(Level* level);
  int Partition(int attr_set);
  int CombinePartitions(int part_x, int part_y);

  std::vector<std::string*> table_;
  int attr_num_;
  int* rhs_;
};


#endif