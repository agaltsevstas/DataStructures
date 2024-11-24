#ifndef Lock_guard_h
#define Lock_guard_h

template <class TMutex>
class Lock_guard
{
    Lock_guard(const Lock_guard&) = delete;
    Lock_guard(Lock_guard&&) noexcept = delete;
    Lock_guard& operator=(const Lock_guard&) = delete;
    Lock_guard& operator=(Lock_guard&&) noexcept = delete;
    
public:
    explicit Lock_guard(TMutex& iMutex): _mutex(iMutex)
    {
        _mutex.Lock();
    }
    
    ~Lock_guard()
    {
        _mutex.Unlock();
    }
    
private:
    TMutex& _mutex;
};

#endif /* Lock_guard_h */
