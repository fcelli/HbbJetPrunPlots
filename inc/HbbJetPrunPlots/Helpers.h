#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <sstream>
#include "Record.h"

// Convert numerical value to string with custom precision
template <typename T>
std::string to_string_with_precision(const T val, const int n = 1);

// Function for sorting collections of record based on their NP number
bool sort_NP_ascending(Record r1, Record r2);

#endif