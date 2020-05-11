#include "argparser.h"
#include <iostream>
#include "utils/utils.h"


void ArgumentParser::parse(int argc, char** argv)
{
    for(int i = 1; i < argc; i++)
	{
        std::string option = argv[i++];
        if (!contains(options, option) || i >= argc)
        {
            help();
            exit(-1);
        }
    
        std::string value = argv[i];
        bool valid_value = false;
		switch(options_type[option])
		{
            case 'b':
				valid_value = is_bool(value);
				break;
			case 'i':
                valid_value = is_int(value);
				break;
			case 'f':
				valid_value = is_float(value);
				break;
			default:
                valid_value = true;
				break;
		}

        if (valid_value)
            options_values[option] = value;
        else
        {
            help();
            exit(-1);
        }
	}
}


void ArgumentParser::add_option(std::string name, char type, std::string default_value, std::string help)
{
    if (!(type == 'b' || type == 'i' || type == 'f' || type == 's'))
        throw "Error: Invalid type type for argument";

    if (contains(options, name))
        throw "Error: Option already exist";

    options.push_back(name);
    options_type.insert(std::pair<std::string, char>(name, type));
    options_help.insert(std::pair<std::string, std::string>(name, help));

    if (default_value == "")
    {
        switch (type)
        {
            case 'b':
                default_value = "true";
                break;
            case 'i':
                default_value = "0";
                break;
            case 'f':
                default_value = "0.0";
                break;
            default:
                break;
        }
    }

    bool valid_value = false;
    switch (type)
    {
        case 'b':
            valid_value = is_bool(default_value);
            break;
        case 'i':
            valid_value = is_int(default_value);
            break;
        case 'f':
            valid_value = is_float(default_value);
            break;
        default:
            valid_value = true;
            break;
    }
    
    if (!valid_value)
        throw "Error: Invalid default value for option " + name;

    options_defaults.insert(std::pair<std::string, std::string>(name, default_value));
    options_values.insert(std::pair<std::string, std::string>(name, default_value));
}


void ArgumentParser::get_option(std::string arg, bool& value)
{
    if (contains(options, arg))
    {
        if (options_values[arg] == "true")
            value = true;
        else
            value = false;
        return;
    }

    throw "Error: invalid option";
}


void ArgumentParser::get_option(std::string arg, int& value)
{
    if (contains(options, arg))
    {
        value = std::stoi(options_values[arg]);
        return;
    }

    throw "Error: invalid option";
}


void ArgumentParser::get_option(std::string arg, float& value)
{
    if (contains(options, arg))
    {
        value = std::stof(options_values[arg]);
        return;
    }

    throw "Error: invalid option";
}


void ArgumentParser::get_option(std::string arg, std::string& value)
{
    if (contains(options, arg))
    {
        value = options_values[arg];
        return;
    }

    throw "Error: invalid option";
}


void ArgumentParser::help()
{
    std::map<std::string, std::string>::iterator it = options_help.begin();
    std::cout << "Parameters:\n";
    std::cout << "============\n";
    std::cout << "Option\t\tDefualt\t\tDescription\n";
	for (std::pair<std::string, std::string> element : options_help)
		std::cout << element.first << '\t' << options_defaults[element.first] << '\t' << element.second << '\n';
}
