#ifndef Logger_h
#define Logger_h

#include <iostream>
#include <sstream>

/*
 Сайты:  https://stackoverflow.com/questions/26143930/xcode-how-to-set-current-working-directory-to-a-relative-path-for-an-executable
    
 Установить путь к проекту: Xcode-> Product-> Scheme-> Edit Scheme-> Run-> Options -> Working Directory-> Use custom working-> $(SOURCE_ROOT)
 */


/*!
 * @brief Класс-синглтон, предназначенный для сбора и сохранения информации о работе программы.
 * Информация записывается в буфер и файлы
 */
class Logger
{
    friend std::default_delete<Logger>;
public:
    
    /*!
     * @brief Четыре уровня подробности лога:
     * DEBUG_LEVEL_DISABLED - Полное отключение лога
     * DEBUG_LEVEL_ERROR - Вывод только ошибок
     * DEBUG_LEVEL_WARNING - Вывод предупреждений и ошибок
     * DEBUG_LEVEL_INFO - Вывод сообщений
     */
    enum DebugLevel
    {
        DEBUG_LEVEL_DISABLED = 0,
        DEBUG_LEVEL_ERROR = 1,
        DEBUG_LEVEL_WARNING = 2,
        DEBUG_LEVEL_INFO = 3,
    };
    
    /*!
     * @brief Четыре уровня вида сообщений:
     * MESSAGE_INFO - Информационное сообщение
     * MESSAGE_WARNING - Предупреждение
     * MESSAGE_ERROR - Ошибка
     * MESSAGE_ALL - Сообщения всех видов (информационное сообщение, предупреждение, ошибка)
     */
    enum MessageType
    {
        MESSAGE_INFO = 1,
        MESSAGE_WARNING,
        MESSAGE_ERROR,
        MESSAGE_ALL
    };
    
    
    /*!
     * @brief Создание единственного объекта
     */
    static void Instance();
    
    /*!
     * @brief Удаление объекта и закрытия лог файла
     */
    static void Delete();
    
    /*!
     * @TODO: Установление уровня подробности лога
     * @param iLevel - Уровень лога
     */
    static void SetDebugLevel(DebugLevel iLevel) noexcept;
    
    /*!
     * @details Производный класс от класссов стандартной библиотеки.
     * Для каждого вида потока создается объект, поэтому конструкторы принимают параметр MessageType.
     * Класс вывода наследуется от ostream, за формирование строки отвечает вложенный класс StringBuffer, который унаследован от stringbuf.
     * После завершениия формирования сообщения, автоматически вызвается метод sync, в котором выполняется непосредственный вывод.
     * Завершение вывода сообщается стандартными для буферизируемых потоков с помощью вызова метода flush
     */
    class Streamer : public std::ostream
    {
    public:
        /*!
         * @brief Консруктор для связывания потока вывода со строковым буфером
         * @param iMessageType - Тип сообщения
         */
        Streamer(Logger::MessageType iMessageType) noexcept;
        
        /*!
         * @brief Деструктор для удаление потока буфера
         */
        ~Streamer();
        
    private:
        class StringBuffer : public std::stringbuf
        {
        public:
            
            /*!
             * @brief Конструктор для отправление типа сообщения
             * @param iMessageType - Тип сообщения
             */
            StringBuffer(Logger::MessageType iMessageType) noexcept;
            
            /*!
             * @brief Деструктор.
             * При разрушении буфера вызывается на всякий случай его синхронизация.
             * Это действие предотвратит «пропадание» сообщения, если забыть вызвать flush или endl.
             */
            ~StringBuffer();
            
            /*!
             * @brief Синхронизация строкового буфера потока
             */
            virtual int sync() override;
            
        private:
            Logger::MessageType _messageType;
        };
    };
    
    inline static Streamer info = Logger::MESSAGE_INFO;       /// Объект потока для информационных сообщений
    inline static Streamer warning = Logger::MESSAGE_WARNING; /// Объект потока для предупреждений
    inline static Streamer error = Logger::MESSAGE_ERROR;     /// Объект потока для ошибок
    
public:
    Logger() {}
    ~Logger();
    
    /*!
     * @brief Запрет копирования
     */
    Logger(const Logger&) = delete;
    
    /*!
     * @brief Запрет оператора присваивания
     */
    Logger& operator=(Logger&) = delete;
    
    /*!
     * @brief Запись информационных сообщений.
     * Запись производится одновременно в файл и буфер в двух потоках
     * @param iMessage - Записываемое сообщение
     */
    void WriteInfo(const std::string& iMessage);
    
    /*!
     * @brief Запись предупреждений.
     * Запись производится одновременно в файл и буфер в двух потоках
     * @param iMessage - Записываемое сообщение
     */
    void WriteWarning(const std::string& iMessage);
    
    /*!
     * @brief Запись ошибок.
     * Запись производится одновременно в файл и буфер в двух потоках
     * @param iMessage - Записываемое сообщение
     */
    void WriteError(const std::string& iMessage);
    
    /*!
     * @brief Запись в буфер.
     * Запись производится в определенный буфер в зависимости от уровня вида сообщения
     * @param iMessage - Записываемое сообщение
     * @param iMessageType - Уровень вида сообщений
     */
    static void WriteToBuffer(const std::string& iMessage, MessageType iMessageType);
    
    /*!
     * @brief Запись в файл
     * @param iMessage - Записываемое сообщение
     */
    void WriteToFile(const std::string& iMessage) noexcept;
    
    /*!
     * @brief Вывод информационных сообщений на экран
     */
    static void PrintInfo() noexcept;
    
    /*!
     * @brief Вывод предупреждений на экран
     */
    static void PrintWarning() noexcept;
    
    /*!
     * @brief Вывод ошибок на экран
     */
    static void PrintError() noexcept;
    
    /*!
     * @brief Вывод всех сообщений (информационные сообщений, предупреждения, ошибки) на экран
     */
    static void PrintAllMessages() noexcept;
    
private:
    inline static std::string _infoBuffer;        /// Буфер для хранения информационных сообщений
    inline static std::string _warningBuffer;     /// Буфер для хранения предупреждений
    inline static std::string _errorBuffer;       /// Буфер для хранения ошибок
    inline static std::string _allMessagesBuffer; /// Буфер для хранения всех видов сообщений
    inline static DebugLevel _debugLevel = DEBUG_LEVEL_DISABLED; /// Уровень подробности лога
    static std::unique_ptr<Logger> _logger; /// Объект-одиночка
    static std::ofstream _logFile;         /// Выходной файловый поток
};

#endif /* Logger_h */
