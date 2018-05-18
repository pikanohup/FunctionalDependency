#include <cstring>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "tane.h"


std::vector<std::string*> ReadData(const std::string file_name, int col_num) {
  std::vector<std::string*> result;
  std::ifstream in_file(file_name);

	if (!in_file) {
		std::cout << "ERROR: Cannot open file" << std::endl;
	}

  std::string line;
  while (getline(in_file, line)) {
    std::string* rows = new std::string[col_num];
    int i = 0;
    int len = line.length();
    int pos = -1, end = 0;

    for (; end < len-1; end++) {
      if (line[end] == ',' && line[end + 1] != ' ') {
        rows[i] = line.substr(pos + 1, end - pos - 1);
        pos = end;
        i++;
      }
    }
    rows[i] = line.substr(pos + 1);

    result.push_back(rows);
  }

  in_file.close();
	return result;
}


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

    std::set<int>::iterator it = level->attr_sets.begin(), end = level->attr_sets.end();
	  while (it != end) {
		  int flag = 1;
      for (int i = 0; i < attr_num; i++) {
			  if (!(*it & flag)) {
				  next_level->attr_sets.insert(*it + flag);
			  }
			  flag <<= 1;
		  }
      it++;
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

  partitions_ = new std::vector<std::vector<int>>[rhs_size];
  memset(partitions_, 0, rhs_size * sizeof(std::vector<std::vector<int>>));

  partition_sizes_ = new int[rhs_size];
  memset(partition_sizes_, 0, rhs_size * sizeof(int));

  partitioned_ = new bool[rhs_size];
  memset(partitioned_, 0, rhs_size * sizeof(bool));

  dependencies_ = std::set<struct Dependency>();
}


void Tane::ComputeDependencies() {
  Level* level = GenerateNextLevel(NULL, attr_num_);
  int count = attr_num_ - 1;
  while (count--) {
		level = GenerateNextLevel(level, attr_num_);
    ComputeDependencies(level);
	}
}


void Tane::Output(const std::string file_name) {
  std::ofstream out_stream(file_name);

  std::set<struct Dependency>::iterator it = dependencies_.begin(), end = dependencies_.end();
  while (it != end) {
    Output(*it, out_stream);
    it++;
  }
}


void Tane::Output(struct Dependency dependency, std::ostream& out_stream) {
  int left_flag = 1;
	for (int j = 1; j <= attr_num_; j++) {
    if (left_flag & dependency.left_attrs) {
			out_stream << j << " ";
		}
		left_flag <<= 1;
	}

  out_stream << "-> ";

  int right_flag = 1;
  for (int i = 1; i <= attr_num_; i++) {
    if (right_flag & dependency.right_attrs) {
      out_stream << i << std::endl;
      break;
		}
		right_flag <<= 1;
	}
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
        if (Validate(x - e, e)) {

          // output (X\{E} -> E)
          dependencies_.insert(Dependency(x - e, e));

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


int Tane::Partition(int attr_set) {
  if (!partitioned_[attr_set]) {
    std::unordered_map<std::string, int> part_map;
    std::vector<std::vector<int>> part_sets;

    int s = attr_set, attr_pos = 0;
    for (; attr_pos < attr_num_; attr_pos++) {
		  if (s & 1) {
			  break;
		  }
		  s >>= 1;
	  }

    int attr_index = 0;
	  for (int i = 0; i < table_.size(); i++) {
		  std::string attr_value = table_[i][attr_pos];
		  std::unordered_map<std::string, int>::iterator map_pos = part_map.find(attr_value);

		  if (map_pos == part_map.end()) {
        part_map.insert(make_pair(attr_value, attr_index));
        attr_index++;
			  std::vector<int> init_set;
			  init_set.push_back(i);
			  part_sets.push_back(init_set);
		  } else {
			  part_sets[map_pos->second].push_back(i);
		  }
	  }

    for (auto it : part_sets) {
		  if (it.size() != 1) {
        partitions_[attr_set].push_back(it);
        partition_sizes_[attr_set] += it.size();
		  }
	  }

    partitioned_[attr_set] = true;
  }

  return partitions_[attr_set].size();
}


int Tane::Partition(int set_x, int set_y) {
  int set_r = set_x + set_y;

  if (!partitioned_[set_r]) {
    int size_x = Partition(set_x), size_y = Partition(set_y);
    std::vector<std::vector<int>> part_sets(size_x);
    int *flags = new int[table_.size()];
	  memset(flags, -1, table_.size() * sizeof(int));

	  for (int i = 0; i < size_x; i++) {
      part_sets.push_back(std::vector<int>());
      for (auto it : partitions_[set_x][i]) {
			  flags[it] = i;
		  }
	  }

	  for (int i = 0; i < size_y; i++) {
		  for (auto it : partitions_[set_y][i]) {
			  if (flags[it] != -1) {
          part_sets[flags[it]].push_back(it);
			  }
		  }

		  for (auto it : partitions_[set_y][i]) {
			  if (flags[it] != -1) {
				  if (part_sets[flags[it]].size() >= 2) {
            partitions_[set_r].push_back(part_sets[flags[it]]);
            partition_sizes_[set_r] += part_sets[flags[it]].size();
				  }
				  part_sets[flags[it]] = std::vector<int>();
			  }
		  }
	  }

    partitioned_[set_r] = true;
  }

  return partitions_[set_r].size();
}


bool Tane::Validate(int set_x, int set_y) {
  int set_r = set_x + set_y;
  int size_x = Partition(set_x);
  int size_r = Partition(set_x, set_y);

  return partition_sizes_[set_x] - size_x == partition_sizes_[set_r] - size_r;
}