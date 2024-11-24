#ifndef Unique_lock_h
#define Unique_lock_h

struct Defer_lock {};

template <class TMutex>
class Unique_lock
{
    Unique_lock(const Unique_lock&) = delete;
    Unique_lock(Unique_lock&&) noexcept = delete;
    Unique_lock& operator=(const Unique_lock&) = delete;
    Unique_lock& operator=(Unique_lock&&) noexcept = delete;
    
public:
    explicit Unique_lock(TMutex& iMutex) : _mutex(iMutex)
    {
        _mutex.Lock();
    }
    
    explicit Unique_lock(TMutex& iMutex, Defer_lock&) : _mutex(iMutex)
    {
        
    }
    
    ~Unique_lock()
    {
        _mutex.Unlock();
    }
    
    void Lock()
    {
        _mutex.Lock();
    }
    
    bool Try_lock()
    {
        return _mutex.Try_lock();
    }
    
    void Unlock()
    {
        _mutex.Unlock();
    }
    
private:
    TMutex& _mutex;
};

#endif /* Unique_lock_h */
