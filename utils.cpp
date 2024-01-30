#include <sstream>
#include <utils.h>

std::vector<std::string> splitFromStrDelim(std::string s,
                                               std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

std::vector<std::string> splitFromCharDelim(const std::string &s, char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;

  while (getline(ss, item, delim)) {
    result.push_back(item);
  }

  return result;
}

std::vector<int> strVectToIntVect(std::vector<std::string> strVect) {
  std::vector<int> intVect;
  for (auto str : strVect) {
    intVect.push_back(std::stoi(str));
  }
  return intVect;
}