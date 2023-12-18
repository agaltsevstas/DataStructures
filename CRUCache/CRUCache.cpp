#include <iostream>
#include <string>
#include <unordered_map>

template <typename TKey, typename TValue>
class LRUCache
{
    struct Node
    {
        TKey key = 0;
        TValue value = 0;
        Node* next = nullptr;
        Node* prev = nullptr;
    };
    
    using Nodes = std::unordered_map<TKey, Node*>;

public:
    LRUCache(int capacity) noexcept;
    ~LRUCache();
    TValue get(const TKey& key);
    void put(const TKey& key, const TValue& value);
    
private:
    void up(Node* node);

private:
    int _capacity = 0;
    Node* _head = nullptr;
    Node* _tail = nullptr;
    Nodes _nodes;
};

template <typename TKey, typename TValue>
LRUCache<TKey, TValue>::LRUCache(int capacity) noexcept : _capacity(capacity)
{
    
}

template <typename TKey, typename TValue>
LRUCache<TKey, TValue>::~LRUCache()
{
    if (_tail)
    {
        while (_tail->prev)
        {
            auto prev = _tail->prev;
            delete _tail;
            _tail = prev;
        }
        
        delete _tail;
    }
}

template <typename TKey, typename TValue>
TValue LRUCache<TKey, TValue>::get(const TKey& key)
{
    if (auto it = _nodes.find(key); it != _nodes.end())
    {
        auto& [_, node] = *it;
        up(node);
        return node->value;
    }

    return TValue();
}

template <typename TKey, typename TValue>
void LRUCache<TKey, TValue>::put(const TKey& key, const TValue& value)
{
    if (_capacity == 0)
        return;
    
    if (auto it = _nodes.find(key); it != _nodes.end())
    {
        auto& [_, node] = *it;
        up(node);
        node->value = value;
    }
    else
    {
        auto head = new Node{key, value, _head};
        if (_head)
        {
            _head->prev = head;
        }
        else
        {
            _tail = head;
        }
        
        _nodes[key] = _head = head;

        if (_nodes.size() > _capacity)
        {
            _nodes.extract(_tail->key);
            _tail = _tail->prev;
            delete _tail->next;
            _tail->next = nullptr;
        }
    }
}

template <typename TKey, typename TValue>
void LRUCache<TKey, TValue>::up(Node* node)
{
    if (node == _head)
        return;
    else if (node == _tail)
        _tail = _tail->prev;
    
    auto next = node->next;
    auto prev = node->prev;
    
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
    
    node->prev = nullptr;
    node->next = _head;
    _head->prev = node;
    _head = node;
}

int main ()
{
    LRUCache<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.put(4, 4);
    std::cout << cache.get(4) << ", ";
    std::cout << cache.get(3) << ", ";
    std::cout << cache.get(2) << ", ";
    std::cout << cache.get(1) << ", ";
    cache.put(5, 5);
    std::cout << cache.get(1) << ", ";
    std::cout << cache.get(2) << ", ";
    std::cout << cache.get(3) << ", ";
    std::cout << cache.get(4) << ", ";
    std::cout << cache.get(5) << ", ";
    return 0;
}
