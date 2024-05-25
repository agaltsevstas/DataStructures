#ifndef function_h
#define function_h

#include <functional>
#include <memory>


namespace first_implementation
{
    template <typename T>
    class function;

    template <typename Result, typename... Args>
    class function<Result(Args...)>
    {
         using Invoke = Result (*)(void*, Args&&...); // Функтор
         using Construct = void (*)(void*, void*); // Конструктор
         using Destroy = void (*)(void*); // Деструктор
        
    public:
        function():
        _invoke(nullptr),
        _construct(nullptr),
        _destroy(nullptr),
        _data(nullptr),
        _size(0)
        {}

        template <typename Functor>
        function(Functor functor):
        _invoke(reinterpret_cast<Invoke>(invoker<Functor>)),
        _construct(reinterpret_cast<Construct>(constructor<Functor>)),
        _destroy(reinterpret_cast<Destroy>(destructor<Functor>)),
        _size(sizeof(Functor)),
        _data(new char[_size]) // выделение памяти под функтор
        {
            _construct(_data.get(), reinterpret_cast<char*>(&functor));
        }
        
        function(const function& other):
        _invoke(other._invoke),
        _construct(other._construct),
        _destroy(other._destroy),
        _size(other._size)
        {
            if (_invoke)
            {
                _data.reset(new char[_size]);
                _construct(_data.get(), other._data.get());
            }
        }
        
        function(function&& other) noexcept
        {
            other.swap(*this);
        }

        function& operator=(const function& other)
        {
            if (this == &other) // object = object
                return *this;
            
            function(other).swap(*this);
            return *this;
        }

        function& operator=(function&& other) noexcept
        {
            if (this == &other) // object = object
                return *this;
            
            function(std::move(other)).swap(*this);
            return *this;
        }

        function& operator=(std::nullptr_t)
        {
            if (_data)
            {
                _size = 0;
                _destroy(_data.get());
                _invoke = nullptr;
                _construct = nullptr;
                _destroy = nullptr;
            }
            
            return *this;
        }

        ~function()
        {
            if (_data)
                _destroy(_data.get());
        }
        
        void swap(function& other) noexcept
        {
            std::swap(_invoke, other._invoke);
            std::swap(_construct, other._construct);
            std::swap(_destroy, other._destroy);
            std::swap(_data, other._data);
            std::swap(_size, other._size);
        }

        Result operator()(Args&&... args)
        {
            if (_invoke)
                return _invoke(_data.get(), std::forward<Args>(args)...); // Вызов функтора с аргументами
            
            return Result();
        }
        
        explicit operator bool() const noexcept { return !!_data; }

    private:
        template <typename Functor>
        static Result invoker(Functor* functor, Args&&... args)
        {
            return (*functor)(std::forward<Args>(args)...); // Вызов функтора с аргументами
        }

        template <typename Functor>
        static void constructor(Functor* ptr, Functor* arg)
        {
            new (ptr) Functor(*arg);
        }

        template <typename Functor>
        static void destructor(Functor* functor)
        {
            functor->~Functor(); // Вызов деструктора вручную, т.к. храним функтор в char[]
        }

        Invoke _invoke = nullptr; // Вызов функтора
        Construct _construct = nullptr; // Вызов конструктора
        Destroy _destroy = nullptr; // Вызов деструктора вручную, т.к. храним функтор в char[]
        size_t _size = 0;
        std::unique_ptr<char[]> _data; // Место где будет храниться функтор
    };
}


namespace second_implementation
{
    template <typename Result, typename ...Args>
    struct Ifunction
    {
        virtual Result operator()(Args&&... args) = 0;
        virtual Ifunction *clone() const = 0;
        virtual ~Ifunction() = default;
    };

    template <typename Function, typename Result, typename ...Args>
    class base_function : public Ifunction<Result, Args...>
    {
    public:
        base_function(const Function& function):
        _function(function)
        {}
        
        Result operator()(Args&&... args) override
        {
            return _function(std::forward<Args>(args)...);
        }

        base_function *clone() const override
        {
            return new base_function(_function);
        }
        
    private:
        Function _function;
    };

    template <typename Function>
    struct function_filter
    {
        using type = Function;
    };

    template <typename Function>
    class function;

    template <typename Result, typename ...Args>
    class function<Result(Args...)>
    {
    public:
        function():
        _function(nullptr)
        {}

        template <typename Function>
        function(const Function& function):
        _function(new base_function<typename function_filter<Function>::type, Result, Args...>(function))
        {}

        function(const function& other):
        _function(other._function ? other._function->clone() : nullptr)
        {}
        
        function(function&& other) noexcept
        {
            other.swap(*this);
        }

        function& operator=(const function& other)
        {
            if ((&other != this) && (other._function)) // object = object
            {
                auto *tmp = other._function->clone();
                delete _function;
                _function = tmp;
            }
            return *this;
        }

        function& operator=(function&& other) noexcept
        {
            if (this == &other) // object = object
                return *this;
            
            _function = std::exchange(other._function, nullptr);
            return *this;
        }

        template<typename Function>
        function& operator=(const Function& function)
        {
            auto tmp = new base_function<typename function_filter<Function>::type, Result, Args...>(function);
            delete _function;
            _function = tmp;
            return *this;
        }

        Result operator()(Args&&... args)
        {
            if (_function)
                return (*_function)(std::forward<Args>(args)...);
            
            return Result();
        }

        ~function()
        {
            if (_function)
            {
                delete _function;
                _function = nullptr;
            }
        }
        
        void swap(function& other) noexcept
        {
            std::swap(_function, other._function);
        }
        
    private:
        Ifunction<Result, Args...> *_function = nullptr;
    };
}

#endif /* function_h */
