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
#include <opencv2/opencv.hpp>
#include <map>
#include <sys/stat.h>
#include <curl/curl.h>

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
    static std::vector<std::string> list_dir(const std::string &name)
    {
        std::vector<std::string> v;
        DIR *dirp = opendir(name.c_str());
        struct dirent *dp;
        std::string file_name;
        while ((dp = readdir(dirp)) != NULL)
        {
            file_name = dp->d_name;
            if (file_name == "." || file_name == "..")
                continue;
            v.push_back(name + '/' + file_name);
        }
        closedir(dirp);

        return v;
    }

    /**
     * @param   int
     * @return  std::string
     * A utility function that generates a random alpha numeric string of any given length
     */
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

    /**
     * @param   const cv::Mat&, std::string
     * @return  std::string
     * A utility function that takes a opencv Mat object and return it's base64 string equivalent
     */
    static std::string img2base64(const cv::Mat &img, std::string img_type = "jpg")
    {
        //Mat to base64
        std::string img_data;
        std::vector<uchar> vecImg;

        std::vector<int> vecCompression_params;
        vecCompression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        vecCompression_params.push_back(90);

        img_type = "." + img_type;
        cv::imencode(img_type, img, vecImg, vecCompression_params);
        img_data = base64_encode(vecImg.data(), vecImg.size());

        return img_data;
    }

    static std::map<std::string, std::string> read_config()
    {
        std::ifstream cFile(".env");
        std::map<std::string, std::string> config;
        if (cFile.is_open())
        {
            std::string line;
            while (getline(cFile, line))
            {
                line.erase(std::remove_if(line.begin(), line.end(), isspace),
                           line.end());
                if (line[0] == '#' || line.empty())
                    continue;
                auto delimiterPos = line.find("=");
                std::string name = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                // std::cout << name << " " << value << '\n';
                config.insert(std::make_pair(name, value));
            }
        }
        else
        {
            std::cerr << "Couldn't open config file for reading.\n";
        }
        return config;
    }

    static bool exists(const std::string &name)
    {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
        size_t written = std::fwrite(ptr, size, nmemb, stream);
        return written;
    }

    static bool download(std::string url, std::string downlaod_path)
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            try
            {
                fp = std::fopen(downlaod_path.c_str(), "wb");
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                res = curl_easy_perform(curl);
                /* always cleanup */
                curl_easy_cleanup(curl);
                std::fclose(fp);
            }
            catch (...)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        return true;
    }

private:
    static std::string base64_encode(const unsigned char *data, int data_byte)
    {
        // code table
        const char symbol_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        //return value
        std::string str_encode;
        unsigned char temp[4] = {0};
        int line_length = 0;
        for (int i = 0; i < (int)(data_byte / 3); i++)
        {
            temp[1] = *data++;
            temp[2] = *data++;
            temp[3] = *data++;
            str_encode += symbol_table[temp[1] >> 2];
            str_encode += symbol_table[((temp[1] << 4) | (temp[2] >> 4)) & 0x3F];
            str_encode += symbol_table[((temp[2] << 2) | (temp[3] >> 6)) & 0x3F];
            str_encode += symbol_table[temp[3] & 0x3F];
            if (line_length += 4, line_length == 76)
            {
                line_length = 0;
            }
        }

        // Encode the remaining data
        int mod = data_byte % 3;
        if (mod == 1)
        {
            temp[1] = *data++;
            str_encode += symbol_table[(temp[1] & 0xFC) >> 2];
            str_encode += symbol_table[((temp[1] & 0x03) << 4)];
            str_encode += "==";
        }
        else if (mod == 2)
        {
            temp[1] = *data++;
            temp[2] = *data++;
            str_encode += symbol_table[(temp[1] & 0xFC) >> 2];
            str_encode += symbol_table[((temp[1] & 0x03) << 4) | ((temp[2] & 0xF0) >> 4)];
            str_encode += symbol_table[((temp[2] & 0x0F) << 2)];
            str_encode += "=";
        }

        return str_encode;
    }
};

#endif
