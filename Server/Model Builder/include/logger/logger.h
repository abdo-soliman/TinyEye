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

    static std::string type2str(MessageType type);
    static std::string error2str(ErrorCode code);
    static std::string warning2str(WarningCode code);

    static void LOG_INFO(std::string message);
    static void LOG_ERROR(ErrorCode code, std::string message);
    static void LOG_WARNING(WarningCode code, std::string message);

private:
    static std::ofstream logfile;
};
} //namespace tinyeye

#endif
