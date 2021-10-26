#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <sstream>
#include <vector>
#include "TString.h"
#include "Record.h"

// Split a string based on some delimiter
std::vector<std::string> split_string(const std::string &str, char delim = ',');

// Convert numerical value to string with custom precision
template <typename T>
std::string to_string_with_precision(const T val, const int n = 1);

// Function for sorting collections of records based on their NP number
bool sort_NP_ascending(Record r1, Record r2);

// Function for sorting collections of <float, TString> pairs in ascending order of the float parameter
bool sort_prun_ascending(std::pair<float, TString> p1, std::pair<float, TString> p2);

#endif