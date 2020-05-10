#ifndef UTILS_H_H_
#define UTILS_H_H_

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

/**
 * @param std::string
 * @return std::vector
 * get file data
*/
std::vector<std::string> get_file_data(std::string input_file_name);
void write_in_csv(std::string input_file_name, float loss, float acc, int Epoch);
void to_lower(std::string &str);
bool is_bool(std::string &s);
bool is_uint(const std::string &s);
bool is_int(const std::string &s);
bool is_float(const std::string &s);
template <typename T>
bool contains(const std::vector<T> &v, const T &value);

void write_in_csv(std::string input_file_name, float loss, float acc, int Epoch = 1)
{
    std::ofstream outfile;

    outfile.open(input_file_name.c_str(), std::ios::app); // append instead of overwrite
    outfile << Epoch << "," << loss << "," << acc << "\n";
    outfile.close();
}

std::vector<std::string> get_file_data(std::string input_file_name)
{
    std::vector<std::string> data;
    std::string line;
    std::ifstream input_file(input_file_name);
    if (input_file.is_open())
    {
        while (getline(input_file, line))
        {
            data.push_back(line);
        }
        input_file.close();
    }
    else
        std::cout << "Unable to open file";

    return data;
}

/**
 * @param   std::string
 * @return  void
 * A utility function to convert string to lower case
 */
void to_lower(std::string &str)
{
    for_each(str.begin(), str.end(), [](char &c) { c = std::tolower(c); });
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
        number = s.substr(1, s.size() - 1);

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
    catch (std::exception &ia)
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
