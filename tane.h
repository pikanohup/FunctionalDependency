#ifndef TANE_H
#define TANE_H

#include <vector>
#include <string>
#include <set>


extern std::vector<std::string*> ReadData(const std::string file_name, int col_num);


struct Level {
	int size;
	std::set<int> attr_sets;
};
extern struct Level* GenerateNextLevel(struct Level* level, int attr_num);


struct Dependency {
  int left_attrs;
  int right_attrs;

  Dependency(int left, int right): left_attrs(left), right_attrs(right) {}

  // IMPROVE
  bool operator < (const struct Dependency &that) const {
    int this_flag = 1, that_flag = 1;

    if (this->left_attrs == that.left_attrs) {
      while (!(this_flag & this->right_attrs)) this_flag <<= 1;
      while (!(that_flag & that.right_attrs)) that_flag <<= 1;
    } else {
      while (!(this_flag & this->left_attrs)) this_flag <<= 1;
      while (!(that_flag & that.left_attrs)) that_flag <<= 1;
    }

    return this_flag < that_flag;
  }
};


class Tane {
 public:
  Tane(const std::string file_name, int attr_num);
  void ComputeDependencies(); 
  void Output(const std::string file_name);

 private:
  void Output(struct Dependency dependency, std::ostream& out_stream);
  void ComputeDependencies(Level* level);
  int Partition(int attr_set);
  int Partition(int set_x, int set_y);
  bool Validate(int set_x, int set_y);

  std::vector<std::string*> table_;
  int attr_num_;
  int* rhs_;
  std::vector<std::vector<int>>* partitions_;
  int* partition_sizes_;
  bool* partitioned_;
  std::set<struct Dependency> dependencies_;
};

#endif