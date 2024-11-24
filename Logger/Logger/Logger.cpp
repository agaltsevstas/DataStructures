#include "Logger.h"

#include <filesystem>
#include <fstream>
#include <thread>

#pragma warning(disable : 4996)


std::string LocalTime()
{
    std::stringstream ss;
    time_t t = std::time(nullptr);
    auto tm = *localtime(&t);
    ss << std::put_time(&tm, "%d.%m.%Y_%H:%M:%S");
    return ss.str();
}

std::unique_ptr<Logger> Logger::_logger = nullptr; // Определение объекта-одиночки
std::ofstream Logger::_logFile; // Определение выходного файлового потока

void Logger::Instance()
{
    if (!_logger)
    {
        _logger.reset(new Logger());
        static Logger instance; // Объект-одиночка
        _logLevel = INFO;
        
        const std::string currrentPath = std::filesystem::current_path().string();
        const std::string fileName = LocalTime() + ".log";
        const std::string directory = "/log/";
        std::filesystem::create_directory(currrentPath + directory); // Проверка на существование каталога. В случае отсутсвия, создается каталог
        std::string filePath = currrentPath + directory + fileName;
        _logFile.open(filePath);
        if (!_logFile.is_open())
            throw std::ofstream::failure("Невозможно открыть директорию >> " + currrentPath + directory);
    }
}

void Logger::SetDebugLevel(LogLevel iLogLevel)
{
    _logLevel = iLogLevel;
}

Logger::Streamer::Streamer(LogLevel iLogLevel)
: std::ostream(new StringBuffer(iLogLevel))
{
}

Logger::Streamer::~Streamer()
{
    delete rdbuf();
}

Logger::Streamer::StringBuffer::StringBuffer(LogLevel iLogLevel) noexcept
: _logLevel(iLogLevel)
{
}

Logger::Streamer::StringBuffer::~StringBuffer()
{
    pubsync();
}

int Logger::Streamer::StringBuffer::sync()
{
    std::string text(str()); // Получение текста из буфера
    if (text.empty())
    {
        return 0;
    }
    str(""); // Очищение буфера
    switch (_logLevel)
    {
        case INFO:
            Logger::_logger->WriteInfo(text);
            break;
            
        case WARNING:
            Logger::_logger->WriteWarning(text);
            break;
            
        case ERROR:
            Logger::_logger->WriteError(text);
            break;
    }
    
    return 0;
};

void Logger::WriteInfo(const std::string& iMessage)
{
    if (_logLevel >= INFO)
    {
        const std::string localTime = "[" + LocalTime() + "] ";
        const std::string str = localTime + iMessage;
        auto thread = std::thread([this, &str]() { this->WriteToFile(str); }); // Отдельный поток, в котром осуществляется запись в файл
        WriteToBuffer(str, INFO);
        thread.join();
    }
}

void Logger::WriteWarning(const std::string& iMessage)
{
    if (_logLevel >= WARNING)
    {
        const std::string localTime = "[" + LocalTime() + "] ";
        const std::string type = "[Warning] ";
        const std::string str = localTime + type + iMessage;
        auto thread = std::thread([this, &str]() { this->WriteToFile(str); }); // Отдельный поток, в котром осуществляется запись в файл
        WriteToBuffer(str, WARNING);
        thread.join();
    }
}

void Logger::WriteError(const std::string& iMessage)
{
    if (_logLevel >= ERROR)
    {
        const std::string localTime = "[" + LocalTime() + "] ";
        const std::string type = "[Error] ";
        const std::string str = localTime + type + iMessage;
        auto thread = std::thread([this, &str]() { this->WriteToFile(str); }); // Отдельный поток, в котром осуществляется запись в файл
        WriteToBuffer(str, ERROR);
        thread.join();
    }
}

void Logger::WriteToBuffer(const std::string& message, LogLevel iLogLevel)
{
    _allMessagesBuffer += message;
    switch (iLogLevel)
    {
        case INFO:
            _infoBuffer += message;
            break;
            
        case WARNING:
            _warningBuffer += message;
            break;
            
        case ERROR:
            _errorBuffer += message;
            break;
    }
}

void Logger::WriteToFile(const std::string& iMessage)
{
    _logFile << iMessage << std::flush; // Принудительный сброс буфера
}

void Logger::PrintInfo()
{
    _infoBuffer.empty() ? std::cout << "Cообщения отсутствуют" << std::endl : std::cout << _infoBuffer;
}

void Logger::PrintWarning()
{
    _warningBuffer.empty() ? std::cout << "Предупреждения отсутствуют" << std::endl : std::cout << _warningBuffer;
}

void Logger::PrintError()
{
    _errorBuffer.empty() ? std::cout << "Ошибки отсутствуют" << std::endl : std::cout << _errorBuffer;
}

void Logger::PrintAllMessages()
{
    std::cout << _allMessagesBuffer << std::endl;
}

Logger::~Logger()
{
    _logFile.flush();
    _logFile.close();
}

