#include "Logger.h"

/*
 Сайты:  https://stackoverflow.com/questions/26143930/xcode-how-to-set-current-working-directory-to-a-relative-path-for-an-executable
    
 Установить путь к проекту: Xcode-> Product-> Scheme-> Edit Scheme-> Run-> Options -> Working Directory-> Use custom working-> $(SOURCE_ROOT)
 */

inline unsigned constexpr Hash(const char *iSource) noexcept
{
    return *iSource ? static_cast<unsigned int>(*iSource) + 33 * Hash(iSource + 1) : 5381;
}

int main(int argc, char *argv[])
{
    Logger::Instance();
    
    Logger::error << "Ошибка" << std::endl;
    Logger::warning << "Предупреждение" << std::endl;
    Logger::info << "Сообщение" << std::endl;
    
    Logger::info << "*********************** Logger *************************" << std::endl;
    while (true)
    {
        std::cout << std::endl;
        std::cout << "*********************** Logger *************************" << std::endl;
        std::cout << "Хотите вывести все информационные сообщения - нажмите 1" << std::endl;
        std::cout << "Хотите вывести все предупреждения - нажмите 2" << std::endl;
        std::cout << "Хотите вывести все ошибки - нажмите 3" << std::endl;
        std::cout << "Хотите вывести весь лог - нажмите 4" << std::endl;
        std::cout << "Хотите вернуться назад? - введите B(англ.) или Н(рус.)" << std::endl;
        std::cout << "Хотите выйти из программы? - введите ESC или ВЫХОД" << std::endl;
        std::cout << "Ввод: ";
        try
        {
            std::string input;
            std::cin >> input;
            switch (Hash(input.c_str()))
            {
                case Hash("1") :
                    Logger::PrintInfo();
                    Logger::info << ">> Вывод всех информационных сообщений <<" << std::endl;
                    break;

                case Hash("2") :
                    Logger::PrintWarning();
                    Logger::info << ">> Вывод всех предупреждений << " << std::endl;
                    break;

                case Hash("3") :
                    Logger::PrintError();
                    Logger::info << ">> Вывод всех ошибок <<" << std::endl;
                    break;

                case Hash("4") :
                    Logger::PrintAllMessages();
                    Logger::info << ">> Вывод всех сообщений <<" << std::endl;
                    break;

                case Hash("esc") :
                case Hash("выход") :
                    exit(0);

                default:
                    throw input;
            }
        }
        catch (const std::string &exception)
        {
            Logger::error << "Введена >> " << exception << " - неверная команда!" << std::endl;
            std::cerr << "Вы ввели >> " << exception << " - неверная команда!" << std::endl;
        }
        catch(const std::exception &ex)
        {
            Logger::error << "Ошибка >> " << ex.what() << std::endl;
            std::cerr << "Ошибка >> " << ex.what() << std::endl;
        }
        catch(...)
        {
            Logger::error << "Неизвестная ошибка!" << std::endl;
            std::cerr << "Неизвестная ошибка!" << std::endl;
            std::exit(0);
        }
    }
    
    return 0;
}
