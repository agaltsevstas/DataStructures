#include "Lock_guard.h"
#include "Unique_lock.h"
#include "Spinlock.h"
#include "Timer.h"

#include <atomic>
#include <iostream>
#include <thread>

Timer timer;

/*
 Spinlock - аналог mutex, который использует цикл активного ожидания (while(true)) и атомарный флаг (std::atomic) входа/выхода из цикла без изменения состояния потока, что приводит к трате процессорного времени на ожидание освобождения блокировки другим потоком, но тратит меньше времени на процедуру блокировки потока, т.к. не требуется задействование планировщика задач (Scheduler) с переводом потока в заблокированное состояние через походы в ядро процессора.
 Методы:
 - lock - происходит захват spinlock текущим потоком и блокирует доступ к данным другим потокам; или поток блокируется, если spinlock уже захвачен другим потоком.
 - unlock - освобождение spinlock, разблокирует данные другим потокам. <br>
 Замечание: При повторном вызове lock - вылезет exception или приведет к состоянию бесконечного ожидания, при повторном вызове unlock - ничего не будет.
 - try_lock - происходит захват spinlock текущим потоком, НО НЕ блокирует доступ к данным другим потокам, а возвращает значение: true - можно захватить spinlock / false - нельзя; НО МОЖЕТ возвращать ложное значение, потому что в момент вызова try_lock spinlock может быть уже lock/unlock. Использовать try_lock в редких случаях, когда поток мог что-то сделать полезное, пока ожидает unlock.
 Плюсы:
 - ожидание захвата блокировки предполагается недолгим.
 - контекст выполнения не позволяет переходить в заблокированное состояние.
 Минусы:
 - при длительной блокировке невыгоден - пустая трата процессорных ресурсов.
 */

template <class TSpinlock>
void Spinlock(const std::string name)
{
    std::cout << name << std::endl;
    TSpinlock spinlock;
    auto PrintSymbol1 = [&](char c)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        spinlock.Lock();
        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << c;
        }
        spinlock.Unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };
    
    auto PrintSymbol2 = [&](char c)
    {
        Lock_guard guard(spinlock);
        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << c;
        }
    };
    
    auto PrintSymbol3 = [&](char c)
    {
        Unique_lock lock(spinlock);
        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << c;
        }
        
        lock.Unlock(); // потому что далее идет задержка
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };
    
    // lock/unlock
    {
        std::cout << "lock/unlock" << std::endl;
        
        timer.start();
        std::thread thread1([&] {PrintSymbol1('+');});
        std::thread thread2([&] {PrintSymbol1('-');});
        
        thread1.join();
        thread2.join();
        timer.stop();
        std::cout << " Время: " << timer.elapsedMilliseconds() << " мс" << std::endl;
        std::cout << std::endl;
    }
    // Lock_guard - делает захват mutex.lock() ресурса на стеке в конструкторе и высвобождает unlock при выходе из стека в деструкторе, используем идиому RAII.
    {
        std::cout << "Lock_guard" << std::endl;
        
        timer.start();
        std::thread thread1([&] {PrintSymbol2('+');});
        std::thread thread2([&] {PrintSymbol2('-');});
        
        thread1.join();
        thread2.join();
        std::cout << " Время: " << timer.elapsedMilliseconds() << " мс" << std::endl;
        std::cout << std::endl;
    }
    // Unique_lock - имеет возможности std::lock_guard + std::mutex
    {
        std::cout << "Unique_lock" << std::endl;
        
        timer.start();
        std::thread thread1([&] {PrintSymbol3('+');});
        std::thread thread2([&] {PrintSymbol3('-');});
        
        thread1.join();
        thread2.join();
        std::cout << " Время: " << timer.elapsedMilliseconds() << " мс" << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
};

int main(int argc, const char * argv[])
{
    // custom::Spinlock
    {
        Spinlock<custom::Spinlock>("custom::Spinlock");
    }
    // cv::Spinlock
    {
        Spinlock<cv::Spinlock>("cv::Spinlock");
    }
    // atomic::Spinlock
    {
        Spinlock<atomic::Spinlock>("atomic::Spinlock");
    }
    // atomicflag::Spinlock
    {
        Spinlock<atomicflag::Spinlock11>("atomicflag::Spinlock11");
        Spinlock<atomicflag::Spinlock20>("atomicflag::Spinlock20");
    }
    
    return 0;
}
