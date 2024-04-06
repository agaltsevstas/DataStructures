#include "Logger.h"

#include <filesystem>
#include <fstream>
#include <thread>


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
        _debugLevel = DEBUG_LEVEL_INFO;
        
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

void Logger::SetDebugLevel(Logger::DebugLevel iDebugLevel) noexcept
{
    _debugLevel = iDebugLevel;
}

Logger::Streamer::Streamer(Logger::MessageType iMessageType) noexcept
: std::ostream(new StringBuffer(iMessageType))
{
}

Logger::Streamer::~Streamer()
{
    delete rdbuf();
}

Logger::Streamer::StringBuffer::StringBuffer(Logger::MessageType iMessageType) noexcept
: _messageType(iMessageType)
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
    switch (_messageType)
    {
        case MESSAGE_INFO:
            Logger::_logger->WriteInfo(text);
            break;
            
        case MESSAGE_WARNING:
            Logger::_logger->WriteWarning(text);
            break;
            
        case MESSAGE_ERROR:
            Logger::_logger->WriteError(text);
            break;
    }
    
    return 0;
};

void Logger::WriteInfo(const std::string& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_INFO)
    {
        const std::string localTime = "[" + LocalTime() + "] ";
        const std::string str = localTime + iMessage;
        auto thread = std::thread([this, &str]() { this->WriteToFile(str); }); // Отдельный поток, в котром осуществляется запись в файл
        WriteToBuffer(str, MessageType::MESSAGE_INFO);
        thread.join();
    }
}

void Logger::WriteWarning(const std::string& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_WARNING)
    {
        const std::string localTime = "[" + LocalTime() + "] ";
        const std::string type = "[Warning] ";
        const std::string str = localTime + type + iMessage;
        auto thread = std::thread([this, &str]() { this->WriteToFile(str); }); // Отдельный поток, в котром осуществляется запись в файл
        WriteToBuffer(str, MessageType::MESSAGE_WARNING);
        thread.join();
    }
}

void Logger::WriteError(const std::string& iMessage)
{
    if (_debugLevel >= DEBUG_LEVEL_ERROR)
    {
        const std::string localTime = "[" + LocalTime() + "] ";
        const std::string type = "[Error] ";
        const std::string str = localTime + type + iMessage;
        auto thread = std::thread([this, &str]() { this->WriteToFile(str); }); // Отдельный поток, в котром осуществляется запись в файл
        WriteToBuffer(str, MessageType::MESSAGE_ERROR);
        thread.join();
    }
}

void Logger::WriteToBuffer(const std::string& message, MessageType iMessageType)
{
    _allMessagesBuffer += message;
    switch (iMessageType)
    {
        case MESSAGE_INFO:
            _infoBuffer += message;
            break;
            
        case MESSAGE_WARNING:
            _warningBuffer += message;
            break;
            
        case MESSAGE_ERROR:
            _errorBuffer += message;
            break;
    }
}

void Logger::WriteToFile(const std::string& iMessage) noexcept
{
    _logFile << iMessage << std::flush; // Принудительный сброс буфера
}

void Logger::PrintInfo() noexcept
{
    _infoBuffer.empty() ? std::cout << "Cообщения отсутствуют" << std::endl : std::cout << _infoBuffer;
}

void Logger::PrintWarning() noexcept
{
    _warningBuffer.empty() ? std::cout << "Предупреждения отсутствуют" << std::endl : std::cout << _warningBuffer;
}

void Logger::PrintError() noexcept
{
    _errorBuffer.empty() ? std::cout << "Ошибки отсутствуют" << std::endl : std::cout << _errorBuffer;
}

void Logger::PrintAllMessages() noexcept
{
    std::cout << _allMessagesBuffer << std::endl;
}

Logger::~Logger()
{
    _logFile.flush();
    _logFile.close();
}

