#ifndef ARGPARSER_H_H_
#define ARGPARSER_H_H_


#include <string>
#include <vector>
#include <map>

class ArgumentParser
{
private:
    std::vector <std::string> options;
    std::map <std::string, char> options_type;
    std::map <std::string, std::string> options_values;
    std::map <std::string, std::string> options_defaults;
    std::map <std::string, std::string> options_help;

public:
    void parse(int argc, char** argv);
    void add_option(std::string name, char type, std::string default_value="", std::string help="none");
    void get_option(std::string arg, bool& value);
    void get_option(std::string arg, int& value);
    void get_option(std::string arg, float& value);
    void get_option(std::string arg, std::string& value);
    void help();
};

#endif
