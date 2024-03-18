# Оглавление
- [Spinlock](#spinlock)
- [Smart Pointers](#smart-pointers)

## Data structures

| Name structure     | Indexation  | Search         | Inserting       | Deleting    | Memory |
|:------------------:|:-----------:|:--------------:|:---------------:|:-----------:|:------:|
| Binary Heap        | -           | -              | O(log(n))       | O(log(n))   | O(n)   |
| Binary Tree        | O(log(n))   | O(log(n))      | O(log(n))       | O(log(n))   | O(n)   |
| LinkedList         | O(n)        | O(n)           | O(1)            | O(1)        | O(n)   |
| Hash Table         | O(1)        | O(1)           | O(1)            | O(1)        | O(n)   |
| Queue              | -           | -              | O(1)            | O(1)        | O(n)   |
| Stack              | -           | -              | O(1)            | O(1)        | O(n)   |

## STL containers

| Name STL container | Indexation  | Search         | Inserting       | Deleting    | Memory | Iterator invalidation | Iterator category |
|:------------------:|:-----------:|:--------------:|:---------------:|:-----------:|:------:|:---------------------:|:------------------|
| array              | O(1)        | -              | -               | -           | O(n)   |           +           |         RA        |
| vector             | O(1)        | -              | O(1) back       | O(n)        | O(n)   |           +           |         RA        |
| list               | O(n)        | O(n)           | O(1)            | O(1)        | O(n)   |           -           |         BD        |
| forward_list       | O(n)        | O(n)           | O(1) front      | O(1) front  | O(n)   |           -           |         F         |
| deque              | O(1)        | O(1)           | O(1) back,front | O(1)        | O(n)   |           +           |         RA        |
| unordered_set      | O(1)        | O(1)           | O(1)            | O(1)        | O(n)   |           +           |         F         |
| unordered_map      | O(1)        | O(1)           | O(1)            | O(1)        | O(n)   |           +           |         F         |
| unordered_multiset | O(1)        | O(1)           | O(1)            | O(1)        | O(n)   |           -           |         F         |
| unordered_multimap | O(1)        | O(1)           | O(1)            | O(1)        | O(n)   |           -           |         F         |
| set                | O(log(n))   | O(log(n))      | O(log(n))       | O(log(n))   | O(n)   |           -           |         BD        |
| map                | O(log(n))   | O(log(n))      | O(log(n))       | O(log(n))   | O(n)   |           -           |         BD        |
| multiset           | O(log(n))   | O(log(n))      | O(log(n))       | O(log(n))   | O(n)   |           -           |         BD        |
| multimap           | O(log(n))   | O(log(n))      | O(log(n))       | O(log(n))   | O(n)   |           -           |         BD        |

## STL adapters
| Name STL adapter   | Indexation  | Search         | Inserting       | Deleting    | Memory |
|:------------------:|:-----------:|:--------------:|:---------------:|:-----------:|:------:|
| queue              | -           | -              | O(1)            | O(1)        | O(n)   |
| stack              | -           | -              | O(1)            | O(1)        | O(n)   |
| priority_queue     | -           | -              | O(log(n))       | O(log(n))   | O(n)   |

## Spinlock
 Spinlock - аналог mutex, который использует цикл активного ожидания (while(true)) и атомарный флаг (std::atomic) входа/выхода из цикла без изменения состояния потока, что приводит к трате процессорного времени на ожидание освобождения блокировки другим потоком, но тратит меньше времени на процедуру блокировки потока, т.к. не требуется задействование планировщика задач (Scheduler) с переводом потока в заблокированное состояние через походы в ядро процессора. <br>
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

## Smart Pointers
RAII (Resource Acquisition Is Initialization) - эффективный способ решения проблемы освобождения ресурсов, в том числе, в случае возникновения исключений. Умные указатели — это RAII классы, которые захватывают ресурс в конструкторе, работают с ним и освобождают его в деструкторе.
Виды:
### Unique_Ptr (сильный указатель - владеет ресурсом)
1 указатель может ссылаться только на один объект, конструктор копирования = delete, оператор копирования = delete. При вызове деструктора удаляет объект.
### Shared_Ptr (сильный указатель - владеет ресурсом)
Производит два раздельных выделения памяти: аллокация управляемого объекта + Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter), вместо одновременного выделения памяти make_shared. При вызове конструктора копирования или оператора копирования ++shared_count, при вызове коструктора перемещения, оператора перемещения или деструктора --shared_count. При shared_count == 0 удаляет объект + weak_count == 0 удаляет Control Block.
 ### Weak_Ptr (слабый указатель - не владаеет ресурсом)
 Создает с помощью lock - Shared_Ptr, но не выделяет память для Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter) - это делает сам Shared_Ptr. При вызове конструктора, конструктора копирования или оператора копирования ++weak_count. При вызове конструктора перемещения, оператора перемещения или деструктора --Weak_Ptr. При shared_count == 0 + Weak_Ptr == 0 удаляет  Control Block, но НЕ УДАЛЯЕТ сам объект, это может сделать ТОЛЬКО - Shared_Ptr. <br>
 Нужен для решения проблемы с «висячим указателем» или «циклической зависимостью», где два объекта взаимноссылаются на друг друга и при выходе видимости стека их деструкторы не будут вызваны -> утечка памяти. Идет в связке с Shared_Ptr, но не увеличивает счетчик ссылок shared_count. <br>
 Из std::Weak_Ptr легко создать умный указатель std::Shared_Ptr, из сырого указателя - это низкоуровневое управление динамической памятью (гиблое дело). <br>
 При создании нескольких std::Shared_Ptr из «сырых указателей» создается свой контрольный блок, они оказываются несвязанные друг с другом и это приводит к двойному удалению объекта. <br>
 При создании нескольких std::Shared_Ptr из std::Weak_Ptr, они будут иметь один контрольный блок. <br>
 При условии, что в std::Weak_Ptr есть объект, с помощью метода lock() создается std::Shared_Ptr. <br>
 expired() - проверяет объект на nullptr. <br>
 
 ## make_unique/make_shared
 Функции, не требующие дублирования типа (auto ptr = make_unique/make_shared<int>(10)). Стоит использовать make_unique/make_shared вместо unique_ptr/shared_ptr<T>(new T()). <br>
make_shared - функция нужна для повышения производительности по сравнению shared_ptr(new), которая с помощью вызова конструктора требуют минимум две аллокации: одну — для размещения объекта, вторую — для Control Block.

Плюсы:
- не нужно писать new.
- там не нужно дублировать тип shared_ptr<int> number(new int(1)) -> auto number = make_shared<int>(1).
- make_shared производит одно выделение памяти вместе: аллокация управляемого объекта + Control Block(shared_count (сильные ссылки), weak_count (слабые ссылки), allocator, deleter), вместо раздельного выделения памяти shared_ptr.

Минусы:
- не могут использовать deleter.
- перегруженные operator new и operator delete в классе будут проигнорированы в make_unique/make_shared.
- make_shared не может вызывать private конструкторы внутри метода (например, синглтон).
- для make_shared нужно ждать shared_count == weak_count == 0, чтобы удалить объект + Control Block.

     
# Лекции:
[shared_ptr, weak_ptr, make_shared, enable_shared_from_this](https://www.youtube.com/watch?v=9ZSBOfTd-sc&ab_channel=%D0%9C%D0%B5%D1%89%D0%B5%D1%80%D0%B8%D0%BD%D0%98%D0%BB%D1%8C%D1%8F) <br/>

# Сайты:
[Ох уж этот std::make_shared…](https://habr.com/ru/articles/509004/) <br/>
[Can you make a std::shared_ptr manage an array allocated with new T?](https://stackoverflow.com/questions/13061979/can-you-make-a-stdshared-ptr-manage-an-array-allocated-with-new-t) <br/>
[What can std::remove_extent be used for?](https://stackoverflow.com/questions/31623862/what-can-stdremove-extent-be-used-for) <br/>
[shared_ptr](https://github.com/google/shipshape/blob/master/third_party/proto/src/google/protobuf/stubs/shared_ptr.h) <br/>
[shared_ptr](https://github.com/yalekseev/shared_ptr/blob/master/shared_ptr.h) <br/>

