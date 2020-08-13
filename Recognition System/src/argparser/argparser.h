#ifndef ARGPARSER_H_H_
#define ARGPARSER_H_H_

#include <map>
#include <string>
#include <vector>

class ArgumentParser
{
public:
    void parse(int argc, char** argv);
    void add_option(std::string name, char type, std::string default_value="", std::string help="none");
    template <typename T> T get_option(std::string arg);
    void help();

private:
    std::vector <std::string> options;
    std::map <std::string, char> options_type;
    std::map <std::string, std::string> options_values;
    std::map <std::string, std::string> options_defaults;
    std::map <std::string, std::string> options_help;

    int longest_option_size = 6;
    int longest_default_size = 7;
};

#endif
