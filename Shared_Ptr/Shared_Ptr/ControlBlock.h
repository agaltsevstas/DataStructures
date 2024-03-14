#ifndef ControlBlock_h
#define ControlBlock_h

namespace STD
{
    class ControlBlock
    {
        template <class TClass> friend class Shared_Ptr;
        template <class TClass> friend class Weak_Ptr;
        
    private:
        ControlBlock() noexcept : _shared_count(1), _weak_count(0) {}
        
    private:
        std::atomic<uint64_t> _shared_count;
        std::atomic<uint64_t> _weak_count;
    };
};

#endif /* ControlBlock_h */
