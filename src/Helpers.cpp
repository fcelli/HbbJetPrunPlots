#include "Helpers.h"

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