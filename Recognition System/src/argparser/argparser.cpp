#include "argparser.h"

#include <iostream>
#include <exception>

#include "utils.h"

void ArgumentParser::parse(int argc, char** argv)
{
    for(int i = 1; i < argc; i++)
	{
        std::string option = argv[i++];
        if (option == "--help")
        {
            help();
            exit(0);
        }

        if (!utils::contains(options, option) || i >= argc)
        {
            help();
            throw std::runtime_error("Error: Invalid option " + option + " doesn't exist");
        }
    
        std::string value = argv[i];
        bool valid_value = false;
		switch(options_type[option])
		{
            case 'b':
				valid_value = utils::is_bool(value);
				break;
			case 'i':
                valid_value = utils::is_int(value);
				break;
			case 'f':
				valid_value = utils::is_float(value);
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
            throw std::runtime_error("Error: Value " + value + " is not a valid value for option " + option);
        }
	}
}

void ArgumentParser::add_option(std::string name, char type, std::string default_value, std::string help)
{
    if (!(type == 'b' || type == 'i' || type == 'f' || type == 's'))
        throw std::runtime_error("Error: Invalid type only (b, i, f, s) are allowed as types");

    if (utils::contains(options, name))
        throw std::runtime_error("Error: Option " + name + " already exist");

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
            valid_value = utils::is_bool(default_value);
            break;
        case 'i':
            valid_value = utils::is_int(default_value);
            break;
        case 'f':
            valid_value = utils::is_float(default_value);
            break;
        default:
            valid_value = true;
            break;
    }
    
    if (valid_value)
    {
        if (type == 'i')
            default_value = std::to_string(std::stoi(default_value));
        else if (type == 'f')
            default_value = std::to_string(std::stof(default_value));
    }
    else
        throw std::runtime_error("Error: Invalid default value for option " + name);


    options_defaults.insert(std::pair<std::string, std::string>(name, default_value));
    options_values.insert(std::pair<std::string, std::string>(name, default_value));

    if (name.size() > longest_option_size)
        longest_option_size = name.size();

    if (default_value.size() > longest_default_size)
        longest_default_size = default_value.size();
}

template <> bool ArgumentParser::get_option(std::string arg)
{
    if (utils::contains(options, arg))
    {
        if (options_values[arg] == "true")
            return true;
        else
            return false;
    }

    throw std::runtime_error("Error: Trying to a non existent option: " + arg);
}

template <> int ArgumentParser::get_option(std::string arg)
{
    if (utils::contains(options, arg))
        return std::stoi(options_values[arg]);

    throw std::runtime_error("Error: Trying to a non existent option: " + arg);
}

template <> float ArgumentParser::get_option(std::string arg)
{
    if (utils::contains(options, arg))
        return std::stof(options_values[arg]);

    throw std::runtime_error("Error: Trying to a non existent option: " + arg);
}

template <> std::string ArgumentParser::get_option(std::string arg)
{
    if (utils::contains(options, arg))
        return options_values[arg];

    throw std::runtime_error("Error: Trying to a non existent option: " + arg);
}

void ArgumentParser::help()
{
    std::map<std::string, std::string>::iterator it = options_help.begin();

    std::cout << "Options:\n";
    std::cout << "=========\n";
    std::cout << "Option" + std::string(longest_option_size - 6, ' ') << '\t';
    std::cout << "Defualt" + std::string(longest_default_size - 7, ' ') << '\t';
    std::cout << "Description\n";

    std::string default_value;
	for (std::pair<std::string, std::string> element : options_help)
    {
        default_value = options_defaults[element.first];
        if (default_value == " ")
            default_value = "' '";
		std::cout << element.first + std::string(longest_option_size - element.first.size(), ' ') << '\t';
        std::cout << default_value + std::string(longest_default_size - default_value.size(), ' ') << '\t';
        std::cout << element.second << '\n';
    }

    std::cout << "--help" + std::string(longest_option_size - 6, ' ') << '\t';
    std::cout << std::string(longest_default_size, ' ') << '\t';
    std::cout << "show this help message\n";
}
