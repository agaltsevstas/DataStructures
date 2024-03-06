#ifndef Spinlock_h
#define Spinlock_h

#include <iostream>
#include <mutex>
#include <thread>

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

// 1 Способ: Простая реализация
namespace custom
{
    class Spinlock
    {
        Spinlock(const Spinlock&) = delete;
        Spinlock &operator = (const Spinlock&) = delete;
        
    public:
        Spinlock() = default;
        ~Spinlock() = default;
        
        void Lock() noexcept
        {
            if (_flag)
            {
                while(_flag);
            }
            else
            {
                _thread_id = std::this_thread::get_id();
                _flag = true;
            }
        }
        
        bool Try_lock() noexcept
        {
            auto lock = _flag.load();
            if (!lock)
                _flag = true;
            
            return !lock;
        }
        
        void Unlock() noexcept
        {
            if (_thread_id == std::this_thread::get_id())
                _flag = false;
        }
        
    private:
        std::thread::id _thread_id;
        std::atomic_bool _flag = false;
    };
}

// 2 Способ: std::condition_variable 
namespace cv
{
    class Spinlock
    {
        Spinlock(const Spinlock&) = delete;
        
    public:
        Spinlock() = default;
        ~Spinlock() = default;
        
        void Lock() noexcept
        {
            std::unique_lock lock(_mutex);
            _cv.wait(lock, [this] { return !_flag.exchange(true); });
        }
        
        bool Try_lock() noexcept
        {
            return !_flag.exchange(true);
        }
        
        void Unlock() noexcept
        {
            _cv.notify_one();
            _flag = false;
        }
        
    private:
        std::atomic<bool> _flag;
        std::mutex _mutex;
        std::condition_variable _cv;
    };
}

// 3 Способ: std::atomic
namespace atomic
{
    // compare_exchange_weak
    namespace compare_exchange_strong
    {
        class Spinlock
        {
            Spinlock(const Spinlock&) = delete;
            
        public:
            Spinlock() = default;
            ~Spinlock() = default;
            
            void Lock() noexcept
            {
                while(_flag);
                bool expected = false;
                _flag.compare_exchange_strong(expected, true);
            }
            
            bool Try_lock() noexcept
            {
                bool expected = false;
                return _flag.compare_exchange_strong(expected, true);
            }
            
            void Unlock() noexcept
            {
                _flag = false;
            }
            
        private:
            std::atomic<bool> _flag = ATOMIC_FLAG_INIT; // false
        };
    }
    
    // compare_exchange_strong
    namespace compare_exchange_weak
    {
        class Spinlock
        {
        public:
            void Lock()
            {
                bool expected = false;
                while (!_flag.compare_exchange_weak(expected, true, std::memory_order_acquire)) // может быть ложное срабатывания
                    expected = false;
            }
            
            void Try_lock()
            {
                bool expected = false;
                _flag.compare_exchange_weak(expected, true, std::memory_order_acquire);
            }
         
            void Unlock()
            {
                _flag.store(false, std::memory_order_release);
            }
         
        private:
            std::atomic<bool> _flag;
        };
    }
}

// 4 Способ: std::atomic_flag
namespace atomic_flag
{
    // С++11
    class Spinlock11
    {
        Spinlock11(const Spinlock11&) = delete;
        
    public:
        Spinlock11() = default;
        ~Spinlock11() = default;
        
        void Lock() noexcept
        {
            while (_flag.test_and_set()) // read
                std::this_thread::yield(); // приостановливает текущий поток, отдав преимущество другим потокам
        }
        
        bool Try_lock() noexcept
        {
            return !_flag.test_and_set();
        }
        
        void Unlock() noexcept
        {
            _flag.clear(); // write
        }
        
    private:
        std::atomic_flag _flag = ATOMIC_FLAG_INIT; // false
    };

    /*
     Лучшее решение
     C++20: имеет характрестики std::condition_variable (wait, notify_one, notify_all)
     */
    class Spinlock20
    {
        Spinlock20(const Spinlock20&) = delete;
        
    public:
        Spinlock20() = default;
        ~Spinlock20() = default;
        
        void Lock() noexcept
        {
            if (_flag.test_and_set(std::memory_order_acquire)) // read
                _flag.wait(true);
        }
        
        bool Try_lock() noexcept
        {
            return !_flag.test_and_set(std::memory_order_acquire);
        }
        
        void Unlock() noexcept
        {
            _flag.clear(std::memory_order_release); // write
            _flag.notify_one();
        }
        
    private:
        std::atomic_flag _flag = ATOMIC_FLAG_INIT; // false
    };
}

#endif /* Spinlock_h */
