#include "logger.h"

#include <ctime>
#include <iostream>
#include <exception>

namespace tinyeye
{
std::ofstream logger::logfile;
std::string logger::logfile_path;

void logger::set_logfile_path(std::string path)
{
    logfile_path = path;
}

std::string logger::type2str(MessageType type)
{
    switch (type)
    {
    case ERROR:
        return "ERROR";
        break;
    case WARNING:
        return "WARNING";
        break;
    case INFO:
        return "INFO";
        break;
    default:
        return "";
        break;
    }
}

std::string logger::error2str(ErrorCode code)
{
    switch (code)
    {
    case INTERNAL_ERROR:
        return "INTERNAL_ERROR";
        break;
    case ATTEMPT_TO_USE_NON_EXISTENT_OPTION:
        return "ATTEMPT_TO_USE_NON_EXISTENT_OPTION";
        break;
    case PASSED_INVALID_VALUE_FOR_OPTION:
        return "PASSED_INVALID_VALUE_FOR_OPTION";
        break;
    case FAILED_TO_INTIALIZE_MOBILE_FACENET:
        return "FAILED_TO_INTIALIZE_MOBILE_FACENET";
        break;
    case FAILED_TO_CREATE_TRAINSET:
        return "FAILED_TO_CREATE_TRAINSET";
        break;
    case FAILED_TO_CREATE_TESTSET:
        return "FAILED_TO_CREATE_TESTSET";
        break;
    case INVALID_CLASSIFIER:
        return "INVALID_CLASSIFIER";
        break;
    case FAIILED_TO_SAVE_CLASSIFIER:
        return "FAIILED_TO_SAVE_CLASSIFIER";
        break;
    case UNKNOWN:
        return "UNKNOWN";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

std::string logger::warning2str(WarningCode code)
{
    switch (code)
    {
    case EMPTY_PHOTO:
        return "EMPTY_PHOTO";
        break;
    case CLASS_DROPED:
        return "CLASS_DROPED";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

void logger::LOG_INFO(std::string message)
{
    if (!logfile.is_open())
        logfile.open(logfile_path.c_str(), std::ios_base::app);

    std::time_t result = std::time(nullptr);
    std::string timestamp = std::asctime(std::localtime(&result));
    timestamp.pop_back();

    logfile << '[' << timestamp << "] INFO: " << message << "\n";
    std::cout << '[' << timestamp << "] INFO: " << message << "\n";
}

void logger::LOG_ERROR(ErrorCode code, std::string message)
{
    if (!logfile.is_open())
        logfile.open(logfile_path.c_str(), std::ios_base::app);

    std::time_t result = std::time(nullptr);
    std::string timestamp = std::asctime(std::localtime(&result));
    timestamp.pop_back();

    logfile << '[' << timestamp << "] ERROR[" + error2str(code) + "]: " << message << "\n";
    std::cout << '[' << timestamp << "] ERROR[" + error2str(code) + "]: " << message << "\n";
}

void logger::LOG_WARNING(WarningCode code, std::string message)
{
    if (!logfile.is_open())
        logfile.open(logfile_path.c_str(), std::ios_base::app);

    std::time_t result = std::time(nullptr);
    std::string timestamp = std::asctime(std::localtime(&result));
    timestamp.pop_back();

    logfile << '[' << timestamp << "] WARNING[" + warning2str(code) + "]: " << message << "\n";
    std::cout << '[' << timestamp << "] WARNING[" + warning2str(code) + "]: " << message << "\n";
}

void logger::json_codes(std::string path)
{
    std::ofstream codes_file;
    codes_file.open(path.c_str());
    if (!codes_file.is_open())
        throw std::runtime_error("failed to create file please make sure correct path is provided");
    
    codes_file << "{\n";

    // adding error_codes
    codes_file << "\t\"ERROR_CODES\": [\n";
    codes_file << "\t\t\"INTERNAL_ERROR\",\n";
    codes_file << "\t\t\"ATTEMPT_TO_USE_NON_EXISTENT_OPTION\",\n";
    codes_file << "\t\t\"PASSED_INVALID_VALUE_FOR_OPTION\",\n";
    codes_file << "\t\t\"FAILED_TO_INTIALIZE_MOBILE_FACENET\",\n";
    codes_file << "\t\t\"FAILED_TO_CREATE_TRAINSET\",\n";
    codes_file << "\t\t\"FAILED_TO_CREATE_TESTSET\",\n";
    codes_file << "\t\t\"INVALID_CLASSIFIER\",\n";
    codes_file << "\t\t\"FAIILED_TO_SAVE_CLASSIFIER\",\n";
    codes_file << "\t\t\"UNKNOWN\"\n";
    codes_file << "\t],\n";

    // adding warning codes
    codes_file << "\t\"WARNING_CODES\": [\n";
    codes_file << "\t\t\"EMPTY_PHOTO\",\n";
    codes_file << "\t\t\"CLASS_DROPED\"\n";
    codes_file << "\t]\n";
    codes_file << "}\n";
}
} //namespace tinyeye
