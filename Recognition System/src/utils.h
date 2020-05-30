#ifndef _UTILS_H_H_
#define _UTILS_H_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <torch/torch.h>

class utils
{
public:
    /**
 * @param std::string
 * @return std::vector
 * get file data
*/
    // static std::vector<std::string> get_file_data(std::string input_file_name);
    // static void write_in_csv(std::string input_file_name, float loss, float acc, int Epoch = 1);
    // static void write_in_file(std::string input_file_name, std::string data);
    // static void to_lower(std::string &str);
    // static bool is_bool(std::string &s);
    // static bool is_uint(const std::string &s);
    // static bool is_int(const std::string &s);
    // static bool is_float(const std::string &s);
    // template <typename T>
    // static bool contains(const std::vector<T> &v, const T &value);
    // static std::vector<std::string> split(std::string src, std::string delimiter);
    static void write_in_csv(std::string input_file_name, float loss, float acc, int Epoch)
    {
        std::ofstream outfile;

        outfile.open(input_file_name.c_str(), std::ios::app); // append instead of overwrite
        outfile << Epoch << "," << loss << "," << acc << "\n";
        outfile.close();
    }
    
    static void append_tensors(std::string input_file_name, torch::Tensor data)
    {
        std::ofstream outfile;

        outfile.open(input_file_name.c_str(), std::ios::app); // append instead of overwrite
        outfile << data;
        outfile.close();
    }

    static void write_in_file(std::string input_file_name, std::string data)
    {
        std::ofstream outfile;

        outfile.open(input_file_name.c_str());
        outfile << data << "\n";
        outfile.close();
    }

    static torch::Tensor get_tensors(std::string input_file_name)
    {
        std::vector<float> data;
        std::string line;
        std::ifstream input_file(input_file_name);
        if (input_file.is_open())
        {
            getline(input_file, line);
            while (getline(input_file, line))
            {
                std::vector<std::string> splitted = split(line, "]");
                float tensor;
                try {
                    if (splitted.size() > 1)
                    {
                        tensor = std::stof(splitted[1]);
                    } else {
                        tensor = std::stof(splitted[0]);
                    }
                    data.push_back(tensor);
                } catch (...) {
                    
                }
            }
            input_file.close();
        }
        else
            std::cout << "Unable to open file";

        auto opts = torch::TensorOptions().dtype(torch::kFloat);
        torch::Tensor tensors = torch::from_blob(data.data(), {data.size()}, opts).to(torch::kFloat);
        return tensors;
    }

    static std::vector<std::string> get_file_data(std::string input_file_name)
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
    static void to_lower(std::string &str)
    {
        for_each(str.begin(), str.end(), [](char &c) { c = std::tolower(c); });
    }

    /**
 * @param   std::string
 * @return  bool
 * A utility function to check if string is an unsigned integer
 */
    static bool is_bool(std::string &s)
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
    static bool is_uint(const std::string &s)
    {
        return !s.empty() && std::find_if(s.begin(),
                                          s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    /**
 * @param   std::string
 * @return  bool
 * A utility function to check if string is an integer
 */
    static bool is_int(const std::string &s)
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
    static bool is_float(const std::string &s)
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
    static bool contains(const std::vector<T> &v, const T &value)
    {
        return std::count(v.begin(), v.end(), value);
    }

    /**
 * @param   std::string, std::string
 * @return  std::vector<std::string>
 * A utility function that splits string into tokens separated by delimiter
 */
    static std::vector<std::string> split(std::string src, std::string delimiter = " ")
    {
        std::vector<std::string> tokens;

        size_t pos = 0;
        std::string token;
        while ((pos = src.find(delimiter)) != std::string::npos)
        {
            tokens.push_back(src.substr(0, pos));
            src.erase(0, pos + delimiter.length());
        }
        tokens.push_back(src);

        return tokens;
    }
};

#endif
