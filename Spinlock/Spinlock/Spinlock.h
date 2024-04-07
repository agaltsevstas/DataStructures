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

/*
 MEMORY ORDER:
 Для оптимизации работы с памятью у каждого ядра имеется его личный кэш памяти, над ним стоит общий кэш памяти процессора, далее оперативная память. Задача синхронизации памяти между ядрами - поддержка консистентного представления данных на каждом ядре (в каждом потоке). Если применить строгую упорядоченность изменений памяти, то операции на разных ядрах уже не будут выполнятся параллельно: остальные ядра будут ожидать, когда одно ядро выполнит изменения данных. Поэтому процессоры поддерживают работу с памятью с менее строгими гарантиями консистентности памяти. Разработчику предоставляется выбор: гарантии по доступу к памяти из разных потоков требуются для достижения максимальной корректности и производительности многопоточной программы.
 Модели памяти в std::atomic - это гарантии корректности доступа к памяти из разных потоков. По-умолчанию компилятор предполагает, что работа идет в одном потоке и код будет выполнен последовательно, но компилятор может переупорядочить команды программы с целью оптимизации. Поэтому в многопоточности требуется соблюдать правила упорядочивания доступа к памяти, что позволяет с синхронизировать потоки с определенной степенью синхронизации без использования дорогостоящего std::mutex.
 */

// По возрастанию строгости
enum memory_order
{
    memory_order_relaxed, // Все можно делать
    memory_order_consume, // Упрощенный memory_order_acquire, гарантирует, что операции будут выполняться в правильном порядке только те, которые зависят от одних данных.
    memory_order_acquire, // Можно пускать вниз операцию STORE, но нельзя операцию LOAD и никакую операцию вверх.
    memory_order_release, // Можно пускать вверх операцию LOAD, но нельзя операцию STORE и никакую операцию вниз.
    memory_order_acq_rel, // memory_order_acquire + memory_order_acq_rel. Можно пускать вниз операцию LOAD, но нельзя операцию STORE. Можно пускать вверх операцию STORE, но нельзя операцию LOAD. Дорогая операция, но дешевле mutex. Используется в x86/64.
    memory_order_seq_cst // Установлено по-умолчанию в atomic, компилятор не может делать переупорядочивание (reordining) - менять порядок в коде. Самая дорогая операция, но дает 100% корректности.
};

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
            // LOAD (no) ↑ STORE (no)
            if (_flag)
            // LOAD (no) ↓ STORE (no)
            {
                // LOAD (no) ↑ STORE (no)
                while(_flag);
                // LOAD (no) ↓ STORE (no)
            }
            else
            {
                _thread_id = std::this_thread::get_id();
                // LOAD (no) ↑ STORE (no)
                _flag = true;
                // LOAD (no) ↓ STORE (no)
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
            {
                // LOAD (no) ↑ STORE (no)
                _flag = false;
                // LOAD (no) ↓ STORE (no)
            }
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
            // LOAD (no) ↑ STORE (no)
            _cv.wait(lock, [this] { return !_flag.exchange(true); }); // замена значения и возвращение старого значения
            // LOAD (no) ↓ STORE (no)
        }
        
        bool Try_lock() noexcept
        {
            // LOAD (no) ↑ STORE (no)
            return !_flag.exchange(true); // замена значения и возвращение старого значения
            // LOAD (no) ↓ STORE (no)
        }
        
        void Unlock() noexcept
        {
            _cv.notify_one();
            // LOAD (no) ↑ STORE (no)
            _flag = false;
            // LOAD (no) ↓ STORE (no)
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
    // compare_exchange_strong
    namespace compare_exchange_weak
    {
        class Spinlock
        {
        public:
            void Lock()
            {
                bool expected = false;
                // LOAD (no) ↑ STORE (no)
                while (!_flag.compare_exchange_weak(expected, true, std::memory_order_acquire)) // сравнивает значение atomic с аргументом expected. Если значения совпадают, то desired записывается в atomic и возвращается true. Если значения НЕ совпадают, то в аргумент expected записывается значение atomic и возвращается false. Возможны ложные пробуждения (spurious wakeup). ложное срабатывания
                // LOAD (no) ↓ STORE (yes)
                    expected = false;
            }
            
            void Try_lock()
            {
                bool expected = false;
                // LOAD (no) ↑ STORE (no)
                _flag.compare_exchange_weak(expected, true, std::memory_order_acquire); // сравнивает значение atomic с аргументом expected. Если значения совпадают, то desired записывается в atomic и возвращается true. Если значения НЕ совпадают, то в аргумент expected записывается значение atomic и возвращается false. Возможны ложные пробуждения (spurious wakeup).
                // LOAD (no) ↓ STORE (yes)
            }
         
            void Unlock()
            {
                // LOAD (yes) ↑ STORE (no)
                _flag.store(false, std::memory_order_release);
                // LOAD (no) ↓ STORE (no)
            }
         
        private:
            std::atomic<bool> _flag;
        };
    }
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
                // LOAD (no) ↑ STORE (no)
                while(_flag);
                // LOAD (no) ↓ STORE (no)
                
                bool expected = false;
                
                // LOAD (no) ↑ STORE (no)
                _flag.compare_exchange_strong(expected, true); // в отличии от compare_exchange_weak включает обработку ложных срабатываний и реализован внутри как вызов compare_exchange_weak в цикле
                // LOAD (no) ↓ STORE (no)
            }
            
            bool Try_lock() noexcept
            {
                bool expected = false;
                
                // LOAD (no) ↑ STORE (no)
                return _flag.compare_exchange_strong(expected, true); // в отличии от compare_exchange_weak включает обработку ложных срабатываний и реализован внутри как вызов compare_exchange_weak в цикле
                // LOAD (no) ↓ STORE (no)
            }
            
            void Unlock() noexcept
            {
                // LOAD (no) ↑ STORE (no)
                _flag = false;
                // LOAD (no) ↓ STORE (no)
            }
            
        private:
            std::atomic<bool> _flag = ATOMIC_FLAG_INIT; // false
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
            // LOAD (no) ↑ STORE (no)
            while (_flag.test_and_set()) // устанавливает значение true и возвращает предыдущее значение
            // LOAD (no) ↓ STORE (no)
            {
                std::this_thread::yield(); // приостановливает текущий поток, отдав преимущество другим потокам
            }
        }
        
        bool Try_lock() noexcept
        {
            // LOAD (no) ↑ STORE (no)
            return !_flag.test_and_set(); // устанавливает значение true и возвращает предыдущее значение
            // LOAD (no) ↓ STORE (no)
        }
        
        void Unlock() noexcept
        {
            // LOAD (no) ↑ STORE (no)
            _flag.clear(); // сбрасывает значение в false
            // LOAD (no) ↓ STORE (no)
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
            // LOAD (no) ↑ STORE (no)
            if (_flag.test_and_set(std::memory_order_acquire)) // устанавливает значение true и возвращает предыдущее значение
            // LOAD (no) ↓ STORE (yes)
            {
                // LOAD (no) ↑ STORE (no)
                    _flag.wait(true);
                // LOAD (no) ↓ STORE (no)
            }
        }
        
        bool Try_lock() noexcept
        {
            // LOAD (no) ↑ STORE (no)
            return !_flag.test_and_set(std::memory_order_acquire); // устанавливает значение true и возвращает предыдущее значение
            // LOAD (no) ↓ STORE (yes)
        }
        
        void Unlock() noexcept
        {
            // LOAD (yes) ↑ STORE (no)
            _flag.clear(std::memory_order_release); // сбрасывает значение в false
            // LOAD (no) ↓ STORE (no)
            
            // LOAD (no) ↑ STORE (no)
            _flag.notify_one(); // снятие блокировки, ожидание 1 потока. Например, 1 писатель или много писателей
            // LOAD (no) ↓ STORE (no)
        }
        
    private:
        std::atomic_flag _flag = ATOMIC_FLAG_INIT; // false
    };
}

#endif /* Spinlock_h */
