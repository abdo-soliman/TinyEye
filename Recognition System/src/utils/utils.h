#ifndef UTILS_H_H_
#define UTILS_H_H_

#include <string>
#include <vector>
#include <algorithm>


void to_lower(std::string& str);
bool is_bool(std::string &s);
bool is_uint(const std::string &s);
bool is_int(const std::string &s);
bool is_float(const std::string &s);

/**
 * @param   std::vector<T>, T
 * @return  bool
 * A utility function to check if vector contains a value
 */
template <typename T>
bool contains(const std::vector<T> &v, const T &value)
{
    return std::count(v.begin(), v.end(), value);
}

#endif
