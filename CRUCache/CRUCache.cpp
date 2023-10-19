#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <unordered_set>

class LRUCache
{
    struct Node
    {
        int key = 0;
        int value = 0;
        Node* next = nullptr;
        Node* prev = nullptr;
    };

public:
    LRUCache(int capacity) : _capacity(capacity)
    {
        
    }

    ~LRUCache()
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
    
    int get(int key)
    {
        if (auto it = _nodes.find(key); it != _nodes.end())
        {
            auto& [_, node] = *it;
            up(node);
            return node->value;
        }

        return -1;
    }
    
    void put(int key, int value)
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
    
private:
    void up(Node* node)
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

private:
    int _capacity = 0;
    Node* _head = nullptr;
    Node* _tail = nullptr;
    std::unordered_map<int, Node*> _nodes;
};

int main ()
{
    LRUCache cache(3);
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
