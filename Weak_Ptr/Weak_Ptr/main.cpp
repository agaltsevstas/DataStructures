#include <iostream>
#include <functional>

#include "Weak_Ptr.h"


/*
 Лекция: https://www.youtube.com/watch?v=9ZSBOfTd-sc&ab_channel=%D0%9C%D0%B5%D1%89%D0%B5%D1%80%D0%B8%D0%BD%D0%98%D0%BB%D1%8C%D1%8F
 Сайты: https://github.com/google/shipshape/blob/master/third_party/proto/src/google/protobuf/stubs/shared_ptr.h
 */

/*
 Shared_Ptr (сильный указатель - владеет ресурсом): производит два раздельных выделения памяти: аллокация управляемого объекта + Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter), вместо одновременного выделения памяти make_shared. При вызове конструктора копирования или оператора копирования ++shared_count, при вызове коструктора перемещения, оператора перемещения или деструктора --shared_count. При shared_count == 0 удаляет объект + weak_count == 0 удаляет Control Block.
 Weak_Ptr (слабый указатель - не владаеет ресурсом): создает с помощью lock - Shared_Ptr, но не выделяет память для Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter) - это делает сам Shared_Ptr. При вызове конструктора, конструктора копирования или оператора копирования ++weak_count. При вызове конструктора перемещения, оператора перемещения или деструктора --Weak_Ptr. При shared_count == 0 + Weak_Ptr == 0 удаляет  Control Block, но НЕ УДАЛЯЕТ сам объект, это может сделать ТОЛЬКО - Shared_Ptr.
 Weak_Ptr нужен для решения проблемы с «висячим указателем» или «циклической зависимостью», где два объекта взаимноссылаются на друг друга и при выходе видимости стека их деструкторы не будут вызваны -> утечка памяти. Идет в связке с Shared_Ptr, но не увеличивает счетчик ссылок.
 из std::Weak_Ptr легко создать умный указатель std::Shared_Ptr, из сырого указателя - это низкоуровневое управление динамической памятью (гиблое дело).
 При создании нескольких std::Shared_Ptr из «сырых указателей» создается свой контрольный блок, они оказываются несвязанные друг с другом и это приводит к двойному удалению объекта.
 При создании нескольких std::Shared_Ptr из std::Weak_Ptr, они будут иметь один контрольный блок.
 При условии, что в std::Weak_Ptr есть объект, с помощью метода lock() создается std::Shared_Ptr.
 expired() - проверяет объект на nullptr.
*/

struct B;

struct A
{
    A() { std::cout << "A()" << std::endl; }
    ~A() { std::cout << "~A()" << std::endl; }
    STD::Shared_Ptr<B> target;
};

struct B
{
    B() { std::cout << "B()" << std::endl; }
    ~B() { std::cout << "~B()" << std::endl; }
    STD::Shared_Ptr<A> target;
};

struct D;

struct C
{
    C() { std::cout << "C()" << std::endl; }
    ~C() { std::cout << "~C()" << std::endl; }
    STD::Weak_Ptr<D> target;
};

struct D
{
    D() { std::cout << "D()" << std::endl; }
    ~D() { std::cout << "~D()" << std::endl; }
    STD::Weak_Ptr<C> target;
};

struct E
{
    E() { std::cout << "E()" << std::endl; }
    ~E() { std::cout << "~E()" << std::endl; }

    STD::Shared_Ptr<E> getTargetCopy()
    {
        /// Проверка на nullptr, если объект != nullptr, то создаем объект с помощью lock();
        if (!target.Expired())
        {
            return target.Lock();
        }
        
        return nullptr;
    }

    STD::Weak_Ptr<E> target;
};


int main()
{
    using namespace STD;

    /// Деструктор для объектов a и b не вызовется из-за закольцеванности
    Shared_Ptr<A> a = Make_Shared<A>();
    Shared_Ptr<B> b = Make_Shared<B>();

    std::cout << "A Use_Count: " << a.Use_Count() << std::endl;
    std::cout << "B Use_Count: " << b.Use_Count() << std::endl;

    a->target = b;
    b->target = a;

    std::cout << "A Use_Count: " << a.Use_Count() << std::endl;
    std::cout << "B Use_Count: " << b.Use_Count() << std::endl;

    std::cout << "------------------------------------------------------------------------" << std::endl;

    /// Деструктор для объектов c и d вызовется
    Shared_Ptr<C> c = Make_Shared<C>();
    Shared_Ptr<D> d = Make_Shared<D>();

    std::cout << "D Use_Count: " << c.Use_Count() << std::endl;
    std::cout << "C Use_Count: " << d.Use_Count() << std::endl;

    c->target = d;
    d->target = c;

    std::cout << "C Use_Count: " << c.Use_Count() << std::endl;
    std::cout << "D Use_Count: " << d.Use_Count() << std::endl;

    /// Деструктор для объектов c и d вызовется даже, если объект ссылается сам на себя
    Shared_Ptr<E> e1 = Make_Shared<E>();
    Shared_Ptr<E> e2 = Make_Shared<E>();

    e1->target = e2;

    std::cout << "e1 Use_Count: " << e1.Use_Count() << std::endl;
    std::cout << "e1 target Use_Count: " << e1->target.Use_Count() << std::endl;

    auto eCopy = e1->getTargetCopy();

    std::cout << "e1 Use_Count: " << e1.Use_Count() << std::endl;
    std::cout << "e1 target Use_Count: " << e1->target.Use_Count() << std::endl;

    e2.Reset();
    eCopy.Reset();

    std::cout << "e1 Use_Count: " << e1.Use_Count() << std::endl;
    std::cout << "e1 target Use_Count: " << e1->target.Use_Count() << std::endl;

    eCopy = e1->getTargetCopy();

    std::cout << "e1 Use_Count: " << e1.Use_Count() << std::endl;
    std::cout << "e1 target Use_Count: " << e1->target.Use_Count() << std::endl;
    
    return 0;
}

