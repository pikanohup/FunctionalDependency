#include <iostream>
#include "tane.h"


struct Level* GenerateNextLevel(struct Level* level, int attr_num) {
  struct Level* next_level = new Level;

  if (level == NULL) {
    next_level->size = 1;

    int flag = 1;
		for (int i = 0; i < attr_num; i++) {
			next_level->attr_sets.insert(flag);
			flag <<= 1;
		}
  } else {
    next_level->size = level->size + 1;

    std::set<int>::iterator end = level->attr_sets.end();
	  for (std::set<int>::iterator it = level->attr_sets.begin(); it != end; it++) {
		  int flag = 1;
      for (int i = 0; i < attr_num; i++) {
			  if (!(*it & flag)) {
				  next_level->attr_sets.insert(*it + flag);
			  }
			  flag <<= 1;
		  }
	  }
  }

  return next_level;
}


Tane::Tane(const std::string file_name, int attr_num) {
  attr_num_ = attr_num;

  table_ = ReadData(file_name, attr_num_);

  int s = 1, rhs_size = (1 << attr_num_);
	rhs_ = new int[rhs_size];
	memset(rhs_, 0, rhs_size * sizeof(int));
	for (int i = 0; i < attr_num_; i++) {
		rhs_[s] = rhs_size - 1;
		s <<= 1;
	}
}


Tane::~Tane() {
}


void Tane::Run() {
  //std::cout << table_[0][10] << std::endl;
}


void Tane::ComputeDependencies(Level* level) {
  std::set<int>::iterator it = level->attr_sets.begin(), end = level->attr_sets.end();
  int rhs_size = (1 << attr_num_);

  // for each X in L do
  while (it != end) {
    int x = *it;
   
    // RHS+(X) := intersect(RHS+(X\{E}), E in X)
    rhs_[x] = rhs_size - 1;
    int e = 1; 
    for (int i = 0; i < attr_num_; i++) {
			if (x & e) {
				rhs_[x] &= rhs_[x - e];
			}
			e <<= 1;
		}

    // for each E in (X intersect RHS+(X)) do
    int inter_x_rhsx = x & rhs_[x];
    e = 1;
		for (int i = 0; i < attr_num_; i++) {
			if (e & inter_x_rhsx) {

        // if (X\{E} -> E) is valid then
				if (707/*TODO*/) {

          // output (X\{E} -> E)
					// ----------TODO-------------

					// remove E from RHS+(X)
					rhs_[x] -= e;

					// remove all F in R\X from RHS+(X)					
					int f = 1, r_minus_x = rhs_size - 1 - x;
					for (int j = 0; j < attr_num_; j++) {
						if (f & r_minus_x) {
							if (f & rhs_[x]) {
								rhs_[x] -= f;
							}
						}
						f <<= 1;
					}
				}
			}
			e <<= 1;
		}

    // if RHS+(X) = empty do
    if (!rhs_[x]) {

      // delete X from L
			level->attr_sets.erase(it++);
		} else {
			it++;
		}
  }
}


int Tane:: Partition(int attr_set) {
  // TODO
  return 707;
}