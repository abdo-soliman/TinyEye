#ifndef UTILS_H_H_
#define UTILS_H_H_

#include <string>
#include <vector>
#include <algorithm>


/**
 * @param   std::string
 * @return  void
 * A utility function to convert string to lower case
 */
void to_lower(std::string& str)
{
	for_each(str.begin(), str.end(), [](char& c) { c = std::tolower(c); });
}

/**
 * @param   std::string
 * @return  bool
 * A utility function to check if string is an unsigned integer
 */
bool is_bool(std::string &s)
{
    to_lower(s);
    if (s == "true" || s == "false")
        return true;

    return false;
}

/**
 * @param   std::string
 * @return  bool
 * A utility function to check if string is an unsigned integer
 */
bool is_uint(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

/**
 * @param   std::string
 * @return  bool
 * A utility function to check if string is an integer
 */
bool is_int(const std::string &s)
{
    std::string number = s;
    if (s[0] == '-' || s[0] == '+')
        number = s.substr(1, s.size()-1);

    return is_uint(number);
}


/**
 * @param   std::string
 * @return  bool
 * A utility function to check if string is a floating point number
 */
bool is_float(const std::string &s)
{
    std::string::size_type sz;
    
    try
    {
        std::stod(s, &sz);
    }
    catch(std::exception& ia)
	{
        return false;
    }
    
    return true;
}

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
