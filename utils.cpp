#include <iostream>
#include <fstream>
#include "utils.h"


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