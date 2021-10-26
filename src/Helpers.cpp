#include "Helpers.h"

std::vector<std::string> split_string(const std::string &str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

template <typename T>
std::string to_string_with_precision(const T val, const int n)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << val;
    return out.str();
}
template std::string to_string_with_precision<float>(float, int);

bool sort_NP_ascending(Record r1, Record r2) {
  return (r1.getNpNum() < r2.getNpNum());
}

bool sort_prun_ascending(std::pair<float, TString> p1, std::pair<float, TString> p2) {
  return (p1.first < p2.first);
}