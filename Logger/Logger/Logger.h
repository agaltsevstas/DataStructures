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
    friend std::default_delete<Logger>; // нужен для std::unique_ptr<Logger>
public:
    /*!
     * @brief Четыре уровня вида сообщений
     * DISABLED - Полное отключение лога
     * INFO - Информационное сообщение
     * WARNING - Предупреждение
     * ERROR - Ошибка
     * ALL - Сообщения всех видов (информационное сообщение, предупреждение, ошибка)
     */
    enum LogLevel
    {
        DISABLED = 0,
        INFO,
        WARNING,
        ERROR,
        ALL
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
     * @param iLogLevel - Уровень лога
     */
    static void SetDebugLevel(LogLevel iLogLevel);
    
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
         * @param iLogLevel - Уровень лога
         */
        Streamer(LogLevel iLogLevel);
        
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
             * @param iLogLevel - Уровень лога
             */
            StringBuffer(LogLevel iLogLevel) noexcept;
            
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
            LogLevel _logLevel;
        };
    };
    
    inline static Streamer info = INFO;       /// Объект потока для информационных сообщений
    inline static Streamer warning = WARNING; /// Объект потока для предупреждений
    inline static Streamer error = ERROR;     /// Объект потока для ошибок
    
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
     * @param iLogLevel - Уровень лога
     */
    static void WriteToBuffer(const std::string& iMessage, LogLevel iLogLevel);
    
    /*!
     * @brief Запись в файл
     * @param iMessage - Записываемое сообщение
     */
    void WriteToFile(const std::string& iMessage);
    
    /*!
     * @brief Вывод информационных сообщений на экран
     */
    static void PrintInfo();
    
    /*!
     * @brief Вывод предупреждений на экран
     */
    static void PrintWarning();
    
    /*!
     * @brief Вывод ошибок на экран
     */
    static void PrintError();
    
    /*!
     * @brief Вывод всех сообщений (информационные сообщений, предупреждения, ошибки) на экран
     */
    static void PrintAllMessages();
    
private:
    inline static std::string _infoBuffer;        /// Буфер для хранения информационных сообщений
    inline static std::string _warningBuffer;     /// Буфер для хранения предупреждений
    inline static std::string _errorBuffer;       /// Буфер для хранения ошибок
    inline static std::string _allMessagesBuffer; /// Буфер для хранения всех видов сообщений
    inline static LogLevel _logLevel = DISABLED;  /// Уровень подробности лога
    static std::unique_ptr<Logger> _logger;       /// Объект-одиночка
    static std::ofstream _logFile;                /// Выходной файловый поток
};

#endif /* Logger_h */
