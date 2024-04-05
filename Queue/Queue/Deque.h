#ifndef Deque_h
#define Deque_h

#include <deque>


namespace DEQUE
{
    template<typename T, class Container = std::deque<T>> // vector, list or deque
    class Queue
    {

    public:
        Queue() = default;
        ~Queue() = default;
        
        Queue(const Queue& other) noexcept
        {
            *this = other;
        }
        
        Queue(Queue&& other) noexcept
        {
            *this = std::move(other);
        }
        
        Queue& operator=(const Queue& other) noexcept
        {
            if (this == &other)
                return *this;
            
            _container = other._container;
            return *this;
        }
        
        Queue& operator=(Queue&& other) noexcept
        {
            if (this == &other)
                return *this;
            
            _container = std::move(other._container);
            return *this;
        }
        
        template<class... Args>
        decltype(auto) Emplace(Args&& ...args)
        {
            return _container.emplace_back(std::forward<Args>(args)...);
        }
        
        template<typename U>
        void Push(U&& value)
        {
            _container.emplace_back(std::forward<U>(value));
        }
        
        T& Front()
        {
            return _container.front();
        }
        
        const T& Front() const
        {
            return _container.front();
        }
        
        T& Back()
        {
            return _container.back();
        }
        
        const T& Back() const
        {
            return _container.back();
        }
        
        void Pop()
        {
            _container.pop_front();
        }
        
        void Swap(Queue& other) noexcept
        {
            if (this == &other)
                return;
            
            std::swap(_container, other._container);
        }
        
        bool Empty() const noexcept
        {
            return _container.empty();
        }
        
        size_t Size() const noexcept
        {
            return _container.size();
        }
        
    private:
        Container _container;
    };
}

#endif /* Deque_h */
