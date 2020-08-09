#include "logger.h"

#include <ctime>
#include <iostream>
#include <exception>

namespace tinyeye
{
    std::ofstream logger::logfile;
    std::string logger::logfile_path;
    sio::client logger::server_socket;
    std::string logger::serial = "board-a8710984-325f-4af3-9c57-d8b84ac39c3e";

    void logger::set_logfile_path(std::string path) { logfile_path = path; }
    void logger::setup_server_socket(std::string url)
    { /*server_socket.connect(url);*/
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
        case FAILED_TO_INTIALIZE_MTCNN:
            return "FAILED_TO_INTIALIZE_MTCNN";
            break;
        case FAILED_TO_INTIALIZE_MOBILE_FACENET:
            return "FAILED_TO_INTIALIZE_MOBILE_FACENET";
            break;
        case FAILED_TO_INTIALIZE_CLASSIFIER:
            return "FAILED_TO_INTIALIZE_CLASSIFIER";
            break;
        case FAILED_TO_CREATE_TEMP_DIR:
            return "FAILED_TO_CREATE_TEMP_DIR";
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

    std::string logger::info2str(InfoCode code)
    {
        switch (code)
        {
        case DETECTED_UNKNOWN:
            return "DETECTED_UNKNOWN";
            break;
        case DETECTED_KNOWN:
            return "DETECTED_KNOWN";
            break;
        default:
            return "UNKNOWN";
            break;
        }
    }

    void logger::LOG_INFO(InfoCode code, std::string message, std::string ref)
    {
        if (!logfile.is_open())
            logfile.open(logfile_path.c_str(), std::ios_base::app);

        std::time_t result = std::time(nullptr);
        std::string timestamp = std::asctime(std::localtime(&result));
        timestamp.pop_back();

        logfile << '[' << timestamp << "] INFO[" + info2str(code) + "]: " << message << "\n";
        std::cout << '[' << timestamp << "] INFO[" + info2str(code) + "]: " << message << "\n";

        // std::string server_log_msg = "{ \"serial\": \"" + serial + "\", ";
        // server_log_msg += "\"type\": \"INFO\", ";
        // server_log_msg += "\"code\": \"" + info2str(code) + "\", ";
        // server_log_msg += "\"timestamp\": \"" + timestamp + "\", ";

        // if (ref != "")
        //     server_log_msg += "\"ref\": \"" + ref + "\", ";

        // server_log_msg += "\"message\": \"" + message + "\" }";
        // server_socket.socket("/" + serial)->emit(serial, server_log_msg);
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

        // std::string server_log_msg = "{ \"serial\": \"" + serial + "\", ";
        // server_log_msg += "\"type\": \"ERROR\", ";
        // server_log_msg += "\"code\": \"" + error2str(code) + "\", ";
        // server_log_msg += "\"timestamp\": \"" + timestamp + "\", ";
        // server_log_msg += "\"message\": \"" + message + "\" }";
        // server_socket.socket("/" + serial)->emit(serial, server_log_msg);
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

        // std::string server_log_msg = "{ \"serial\": \"" + serial + "\", ";
        // server_log_msg += "\"type\": \"WARNING\", ";
        // server_log_msg += "\"code\": \"" + warning2str(code) + "\", ";
        // server_log_msg += "\"timestamp\": \"" + timestamp + "\", ";
        // server_log_msg += "\"message\": \"" + message + "\" }";
        // server_socket.socket("/" + serial)->emit(serial, server_log_msg);
    }
} //namespace tinyeye
