#ifndef Lock_guard_h
#define Lock_guard_h

template <class TMutex>
class Lock_guard
{
    Lock_guard(const Lock_guard&) = delete;
    
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
