#ifndef LOGGER_H_H_
#define LOGGER_H_H_

#include <string>
#include <fstream>

namespace tinyeye
{

enum MessageType
{
    ERROR,
    WARNING,
    INFO
};

enum ErrorCode
{
    INTERNAL_ERROR,
    ATTEMPT_TO_USE_NON_EXISTENT_OPTION,
    PASSED_INVALID_VALUE_FOR_OPTION,
    FAILED_TO_INTIALIZE_MOBILE_FACENET,
    FAILED_TO_CREATE_TRAINSET,
    FAILED_TO_CREATE_TESTSET,
    INVALID_CLASSIFIER,
    FAIILED_TO_SAVE_CLASSIFIER,
    UNKNOWN
};

enum WarningCode
{
    EMPTY_PHOTO,
    CLASS_DROPED
};

class logger
{
public:
    logger() = delete;
    static void set_logfile_path(std::string path);

    static std::string type2str(MessageType type);
    static std::string error2str(ErrorCode code);
    static std::string warning2str(WarningCode code);

    static void LOG_INFO(std::string message);
    static void LOG_ERROR(ErrorCode code, std::string message);
    static void LOG_WARNING(WarningCode code, std::string message);

    static void json_codes(std::string path);

private:
    static std::ofstream logfile;
    static std::string logfile_path;
};
} //namespace tinyeye

#endif
