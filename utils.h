#include <string>
#include <vector>

std::vector<std::string> splitFromStrDelim(std::string s,
                                           std::string delimiter);
std::vector<std::string> splitFromCharDelim(const std::string &s, char delim);
std::vector<int> strVectToIntVect(std::vector<std::string>);