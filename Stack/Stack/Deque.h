#ifndef Deque_h
#define Deque_h

#include <deque>

namespace DEQUE
{
    template<typename T, class Container = std::deque<T>> // vector, list or deque
    class Stack
    {

    public:
        Stack() = default;
        ~Stack() = default;
        
        Stack(const Stack& other) noexcept
        {
            *this = other;
        }
        
        Stack(Stack&& other) noexcept
        {
            *this = std::move(other);
        }
        
        Stack& operator=(const Stack& other) noexcept
        {
            if (this == &other)
                return *this;
            
            _container = other._container;
            return *this;
        }
        
        Stack& operator=(Stack&& other) noexcept
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
        
        T& Top()
        {
            return _container.back();
        }
        
        const T& Top() const
        {
            return _container.back();
        }
        
        void Pop()
        {
            _container.pop_back();
        }
        
        void Swap(Stack& other) noexcept
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
