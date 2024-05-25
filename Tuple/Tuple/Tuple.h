#ifndef Tuple_h
#define Tuple_h

#include <iostream>

/* EBO (empty base optimization) - гарантирует размер любого объекта/подъекта должен быть не менее 1 байта, даже если тип является пустым, чтобы можно было получить разные адреса разных объектов одного и того же типа.
 Атрибут [[no_unique_address]] применяет ту же логику к любому полю структуры, что и EBO. Без этого атрибута размер HList<dummy<0>, dummy<1>, dummy<2>> составлял бы 3 байта вместо 1.
 Несмотря на оптимизацию, размер Tuple<dummy<0>, dummy<0>, dummy<0>> по-прежнему составляет 3 байта, поскольку каждому экземпляру одного и того же типа dummy<0> требуется уникальный адрес. В этом есть определенный смысл: например, конструктор dummy<0> может решить ввести адрес объекта в некоторую глобальную хеш-таблицу, хотя dummy<1> и dummy<0> предположительно будут иметь разные хеш-таблицы, ввод одного и того же адреса дважды в dummy<0> хеш-таблицу может привести к путанице.
 */

namespace tuple
{
    template<std::size_t N> struct dummy{};

    namespace first_implementation
    {
        /// Stub-функция (заглушка) для рекурсии
        template <typename... T>
        class Tuple
        {};

        /// (CTAD) Class template argument deduction - способность шаблонов определять тип передаваемых аргументов без явного указания типа: вместо foo<...>(...) можно foo(...). До C++17 при инстанцировании шаблона функции нужно было явно указывать типы аргументы: foo<...>(...).
        template <typename ...T> Tuple(T...) -> Tuple<T...>;

        /// Полная специализация для пустого tuple
        template<>
        class Tuple<>
        {
        public:
            constexpr static size_t value = 0u;
        };

        /*
         Рекурсиваня специализация, Пример:
         Tuple<int, char, std::string>
         -> int first
         -> Tuple<char, std::string> rest
             -> char first
             -> Tuple<std::string> rest
                 -> std::string first
                 -> Tuple<> rest
                     -> (empty)
         */

        template <typename Head, typename... Tail>
        class Tuple<Head, Tail...>
        {
            template<size_t index, typename H> friend struct GetHelper;
        
        public:
            constexpr Tuple(Head&& head, Tail&& ...tail) :
            _head(std::forward<Head>(head)),
            _tail(std::forward<Tail>(tail)...) {}
            
            Tuple(const Tuple&) = default;
            Tuple(Tuple&&) = default;
            
            template<typename UHead, typename ...UTail>
            constexpr Tuple(UHead &&head, UTail&& ...tail):
            _head(std::forward<UHead>(head)),
            _tail(std::forward<UTail>(tail)...) {}
            
            constexpr size_t Size() const { return value; }
        public:
            constexpr static size_t value = 1u + Tuple<Tail...>::value;
            
        private:
            /// EBO (empty base optimization) - гарантирует размер любого объекта/подъекта должен быть не менее 1 байта, даже если тип является пустым, чтобы можно было получить разные адреса разных объектов одного и того же типа.
            [[no_unique_address]] Head _head;
            Tuple<Tail...> _tail;
        };

        template <typename T>
        class Tuple<T>
        {
            template<size_t index, typename H> friend struct GetHelper;
        public:
            Tuple(const T& value): _head(value)
            {}

            constexpr static size_t value = 1u;

        private:
            [[no_unique_address]] T _head;
        };

        /// Полная специализация для пустого tuple
        template <size_t index, typename T>
        struct GetHelper;

        /// Частичная специализация для index == 0, возврат только первого члена tuple
        template <typename Head, typename... Tail>
        struct GetHelper<0, Tuple<Head, Tail...>>
        {
            static Head Get(const Tuple<Head, Tail...>& tuple)
            {
                return tuple._head;
            }
        };

        /// Метафункция с частичной специализацией для index == n, рекурсионный вызов до n-- > 0
        template <size_t index, typename Head, typename... Tail>
        struct GetHelper<index, Tuple<Head, Tail...>>
        {
            static auto Get(const Tuple<Head, Tail...>& tuple)
            {
                return GetHelper<index - 1, Tuple<Tail...>>::Get(tuple._tail);
            }
        };

        // Функция, вызывающая метафункцию
        template <size_t index, template <typename...> class Tuple, typename... Args>
        auto Get(const Tuple<Args...>& tuple)
        {
            return GetHelper<index, Tuple<Args...>>::Get(tuple);
        }

        template <typename ...Args>
        constexpr Tuple<Args...> Make_Tuple(Args&& ...args)
        {
            return Tuple(std::forward<Args>(args)...);
        }
    }


    namespace second_implementation
    {
        template<typename... Types>
        class Tuple;

        /// Полная специализация для пустого tuple
        template<>
        class Tuple<>
        {
        public:
            constexpr static size_t value = 0u;
        };

        template <typename Head, typename... Tail>
        class Tuple<Head, Tail...> : Tuple<Tail...>
        {
            template<size_t index, typename H, typename... T> friend struct GetHelper;
            
        public:
            Tuple(Head&& head, Tail&&... tail):
            Tuple<Tail...>(std::forward<Tail>(tail)...),
            _head(std::forward<Head>(head))
            {}
            
            Tuple(const Tuple&) = default;
            Tuple(Tuple&&) = default;
            
            constexpr Tuple() = default;
            template<typename UHead, typename ...UTail>
            constexpr Tuple(UHead &&head, UTail&& ...tail):
            Tuple<Tail...>(std::forward<UTail>(tail)...),
            _head(std::forward<UHead>(head))
            {}
            
            constexpr size_t Size() const { return value; }
        public:
            constexpr static size_t value = 1u + Tuple<Tail...>::value;
            
        private:
            /// EBO (empty base optimization) - гарантирует размер любого объекта/подъекта должен быть не менее 1 байта, даже если тип является пустым, чтобы можно было получить разные адреса разных объектов одного и того же типа.
            [[no_unique_address]] Head _head;
        };

        template<typename Head>
        class Tuple<Head>
        {
            template<size_t index, typename H, typename... T> friend struct GetHelper;
        public:
            Tuple(Head head):
            _head(head)
            {}
            
            constexpr static size_t value = 1u;
            
        private:
            /// EBO (empty base optimization) - гарантирует размер любого объекта/подъекта должен быть не менее 1 байта, даже если тип является пустым, чтобы можно было получить разные адреса разных объектов одного и того же типа.
            [[no_unique_address]] Head _head;
        };
        
        // template argument deduction guide
        template<typename ...T> Tuple(T...) -> Tuple<T...>;

        template<size_t index, typename Head, typename... Tail>
        struct GetHelper
        {
            static auto value(const Tuple<Head, Tail...>& tuple)
            {
                return GetHelper<index - 1, Tail...>::value(tuple);
            }
        };

        template<typename Head, typename... Tail>
        struct GetHelper<0, Head, Tail...>
        {
            static Head value(const Tuple<Head, Tail...>& tuple)
            {
                return tuple._head;
            }
        };

        template<size_t index, typename Head, typename... Tail>
        auto Get(const Tuple<Head, Tail...>& tuple)
        {
            return GetHelper<index, Head, Tail...>::value(tuple);
        }

        template <typename ...Args>
        Tuple<Args...> Make_Tuple(Args&& ...args)
        {
            return Tuple(std::forward<Args>(args)...);
        }
    }

    namespace third_implementation
    {
        template <size_t... size>
        struct IndexSequence
        {};

        template <size_t... size>
        struct MakeIndexSequenceImpl;

        template <size_t... size>
        struct MakeIndexSequenceImpl<0, size...>
        {
            using Type = IndexSequence<size...>;
        };

        template <size_t index, size_t... size>
        struct MakeIndexSequenceImpl<index, size...> : MakeIndexSequenceImpl<index - 1, index - 1, size...> {};

        template <size_t index>
        using MakeIndexSequence = typename MakeIndexSequenceImpl<index>::Type;

        template <class P>
        struct TupleTraits
        {
            typedef P ValueType;
            typedef P& RefType;
            typedef const P& ParamType;
        };

        template <class P>
        struct TupleTraits<P&>
        {
            typedef P ValueType;
            typedef P& RefType;
            typedef P& ParamType;
        };

        template <typename index, typename... Args>
        struct TupleBaseImpl;

        template <typename... Args>
        using TupleBase = TupleBaseImpl<MakeIndexSequence<sizeof...(Args)>, Args...>;

        template <typename... Args>
        struct Tuple : TupleBase<Args...>
        {
            Tuple() : TupleBase<Args...>() {}
            explicit Tuple(typename TupleTraits<Args>::ParamType... args):
            TupleBase<Args...>(args...) {}
            
            constexpr size_t Size() const { return value; }
        public:
            constexpr static size_t value = sizeof...(Args);
        };
        
        template <>
        struct Tuple<>
        {
            constexpr static size_t value = 0u;
        };

        // template argument deduction guide
        template<typename ...T> Tuple(T...) -> Tuple<T...>;

        template <size_t index, typename Leaf>
        struct TupleLeaf
        {
            TupleLeaf() {}
            explicit TupleLeaf(typename TupleTraits<Leaf>::ParamType leaf) : _leaf(leaf) {}
            Leaf& Get() { return _leaf; }
            const Leaf& Get() const { return _leaf; }
            
            /// EBO (empty base optimization) - гарантирует размер любого объекта/подъекта должен быть не менее 1 байта, даже если тип является пустым, чтобы можно было получить разные адреса разных объектов одного и того же типа.
            [[no_unique_address]] Leaf _leaf;
        };

        template <size_t... index, typename... Args>
        struct TupleBaseImpl<IndexSequence<index...>, Args...> : TupleLeaf<index, Args>...
        {
            TupleBaseImpl() : TupleLeaf<index, Args>()... {}
            explicit TupleBaseImpl(typename TupleTraits<Args>::ParamType... args):
            TupleLeaf<index, Args>(args)... {}
        };

        template <size_t index, typename T>
        T& Get(TupleLeaf<index, T>& leaf)
        {
            return leaf.Get();
        }

        template <size_t index, typename T>
        const T& Get(const TupleLeaf<index, T>& leaf)
        {
            return leaf.Get();
        }

        template <typename ...Args>
        inline Tuple<Args...> Make_Tuple(Args&& ...args)
        {
            return Tuple(std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline Tuple<Args&...> Make_Ref_Tuple(Args&... args)
        {
            return Tuple<Args&...>(args...);
        }
    }
}

#endif /* Tuple_h */
