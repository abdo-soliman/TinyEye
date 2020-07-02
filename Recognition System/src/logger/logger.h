#ifndef LOGGER_H_H_
#define LOGGER_H_H_

#include <string>
#include <fstream>
#include <sio_client.h>


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
    FAILED_TO_INTIALIZE_MTCNN,
    FAILED_TO_INTIALIZE_MOBILE_FACENET,
    FAILED_TO_INTIALIZE_CLASSIFIER,
    FAILED_TO_CREATE_TEMP_DIR,
    UNKNOWN
};

enum WarningCode
{
    EMPTY_PHOTO,
    CLASS_DROPED
};

enum InfoCode
{
    DETECTED_UNKNOWN,
    DETECTED_KNOWN
};

class logger
{
public:
    logger() = delete;
    static void set_logfile_path(std::string path);
    static void setup_server_socket(std::string url);

    static std::string type2str(MessageType type);
    static std::string error2str(ErrorCode code);
    static std::string warning2str(WarningCode code);
    static std::string info2str(InfoCode type);

    static void LOG_INFO(InfoCode code, std::string message);
    static void LOG_ERROR(ErrorCode code, std::string message);
    static void LOG_WARNING(WarningCode code, std::string message);

private:
    static std::ofstream logfile;
    static std::string logfile_path;
    static sio::client server_socket;
    static std::string serial;
};
} //namespace tinyeye

#endif
