#include <iostream>

/*
 Сайты: https://habr.com/ru/companies/oleg-bunin/articles/352280/
        https://xakep.ru/2014/11/01/akademiya-c-urok-2/
 */

template <typename T>
class copy_on_write
{
public:
    explicit copy_on_write(T* iValue):
    value(iValue)
    {}
    
    T& operator*()
    {
        detach();
        return *value;
    }
    
    const T& operator*() const
    {
        return *value;
    }
    
    T* operator->()
    {
        detach();
        return value.get();
    }
    
    const T* operator->() const
    {
        return value.get();
    }
    
private:
    void detach()
    {
        T* tmp = value.get();
        if (tmp && !(value.use_count() == 1))
        {
            value = std::make_shared<T>(*tmp);
        }
            
    }
    
private:
    std::shared_ptr<T> value;
};


int main()
{
    // string: отсутствие copy on write
    {
        std::cout << "string" << std::endl;
        std::string str1 = "str";
        const std::string str2 = str1;
        
        std::cout << "str1: " << str1 << std::endl;
        std::cout << "str1: " << &(str1) << std::endl;
        
        std::cout << "str2: " << str2 << std::endl;
        std::cout << "str2: " << &(str2) << std::endl;
        
        std::cout << std::endl;
    }
    /*
     COW (copy on write) - копирование при записи. Перед изменением данных объекта сначала происходит копирование в новое место памяти, а потом изменение данных по новому адресу. Для const объектов память не выделяется, а увеличивается счетчик ссылок (можно использовать shared_ptr), таким образом дан­ные мож­но раз­делять меж­ду нес­коль­кими объ­екта­ми, откла­дывая копиро­вание.
     Плюсы:
     - при const объектах не выделяется память, а увеличивается только счетчик ссылок.
     Минусы:
     - с появлением rvalue reference, string_view в стандарте C++11, COW не нужен.
     - нужен атомарный счетчик ссылок.
     */
    {
        std::cout << "copy on write" << std::endl;
        
        const copy_on_write<std::string> cow_string(new std::string("Test"));
        std::cout << "Content cow_string: " << *cow_string << std::endl;
        std::cout << "Adress cow_string: " << &(*cow_string) << std::endl;
        
        const copy_on_write<std::string> const_cow_string = cow_string;
        std::cout << "Content const_cow_string: " << *const_cow_string << std::endl;
        std::cout << "Adress const_cow_string: " << &(*const_cow_string) << std::endl;
        
        copy_on_write<std::string> cow_string_copy = cow_string;
        std::cout << "Content const_cow_string: " << *cow_string_copy << std::endl;
        std::cout << "Adress const_cow_string: " << &(*cow_string_copy) << std::endl;
        
        cow_string_copy->append(" text");
        std::cout << "Content cow_string: " << *cow_string << std::endl;
        std::cout << "Adress cow_string: " << &(*cow_string) << std::endl;
        std::cout << "Content cow_string_copy: " << *cow_string_copy << std::endl;
        std::cout << "Adress cow_string2_copy: " << &(*cow_string_copy) << std::endl;
        
        std::cout << std::endl;
    }
    
    return 0;
}
