#ifndef _UTILS_H_H_
#define _UTILS_H_H_

#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

class utils
{
public:
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

    /**
     * @param   std::string, std::string, std::string
     * @return  std::string
     * A utility function that replaces all occurances of to_replace with replacement
     */
    static std::string replace_all(std::string src, std::string to_replace, std::string replacement = "")
    {
        std::string result = src;
        std::string::size_type n = 0;
        while ((n = result.find(to_replace, n)) != std::string::npos)
        {
            result.replace(n, to_replace.size(), replacement);
            n += replacement.size();
        }

        return result;
    }

    /**
     * @param   std::vector<T>&
     * @return  void
     * A utility function that shuffles a vector
     */
    template <typename T>
    static void shuffle_vec(std::vector<T> &vec)
    {
        unsigned seed = std::chrono::system_clock::now()
                            .time_since_epoch()
                            .count();
        shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
    }

    /**
     * @param   std::string
     * @return  bool
     * A utility function that takes a string directory path and check if the directory exist
     */
    static bool is_dir(std::string dir_path)
    {
        struct stat info;

        if (stat(dir_path.c_str(), &info) != 0)
            return false;

        if (info.st_mode & S_IFDIR)
            return true;

        return false;
    }

    /**
     * @param   std::string
     * @return  std::vector<std::string>
     * A utility function that obtians all files paths in a directory
     */
    static std::vector<std::string> list_dir(const std::string& name)
    {
        std::vector<std::string> v;
        DIR* dirp = opendir(name.c_str());
        struct dirent * dp;
        std::string file_name;
        while ((dp = readdir(dirp)) != NULL) {
            file_name = dp->d_name;
            if (file_name == "." || file_name == "..")
                continue;
            v.push_back(name + '/' + file_name);
        }
        closedir(dirp);

        return v;
    }

    static std::string random_string(int length)
    {
        std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(str.begin(), str.end(), std::default_random_engine(seed));

        std::string result = "";
        if (length < str.size())
            result += str.substr(0, length);
        else if (length == str.size())
            result = str;
        else
        {
            while (length > 0)
            {
                result += str.substr(0, length);
                length -= str.size();
                seed = std::chrono::system_clock::now().time_since_epoch().count();
                shuffle(str.begin(), str.end(), std::default_random_engine(seed));
            }
        }

        return result;
    }
};

#endif
