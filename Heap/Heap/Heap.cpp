#include <iostream>
#include <vector>

// top: O(1)
// push: O(logn)
// popTop: O(logn)
// heapify: O(n)

template <typename Type,
typename Vector = std::vector<Type>,
typename Greater = std::greater<Type>,
typename Greater_Equal = std::greater_equal<Type>>
class heap : public Vector
{
private:
    using Vector::push_back;
    using Vector::emplace_back;
    using Vector::pop_back;
    using Vector::insert;
    using index = int;
    
public:
    heap() = default;
    
    heap(const std::initializer_list<Type>& array) : Vector(array)
    {
        heapify();
    }
    
    Type& top()
    {
        return Vector::front();
    }

    void pop()
    {
        Vector::pop_back();
    }
    
    void popTop()
    {
        std::swap(top(), Vector::back());
        pop();
        shift_down();
    }
    
    void push(const Type& value)
    {
        Vector::push_back(value);
        index last = (int)Vector::size() - 1;
        shift_up(last);
    }
    
private:
    void heapify ()
    {
        for (index i = (index)this->size() - 1; i >= 0; --i)
        {
            shift_up(i);
        }
        
        /*
        for (index i = 0; i < (index)this->size(); ++i)
        {
            shift_down(i);
        }
        */
    }
    
    void shift_down(index first_index = 0)
    {
        index size = (index)this->size();
        while (first_index < size)
        {
            index left_index = first_index * 2 + 1;
            index right_index = first_index * 2 + 2;
            
            if (Greater_Equal()(left_index, size))
            {
                break;
            }
            else if (Greater_Equal()(right_index, size))
            {
                std::swap(this->at(left_index), this->at(first_index));
                break;
            }
            else if (Greater()(this->at(left_index), this->at(right_index)) &&
                     this->at(left_index) > this->at(first_index))
            {
                std::swap(this->at(left_index), this->at(first_index));
                first_index = left_index;
            }
            else if (Greater()(this->at(right_index), this->at(left_index)) &&
                     Greater()(this->at(right_index), this->at(first_index)))
            {
                std::swap(this->at(right_index), this->at(first_index));
                first_index = right_index;
            }
            else
            {
                break;
            }
        }
    }
    
    void shift_up(index last_index)
    {
        while (last_index != 0)
        {
            index parent_index = (last_index - 1) / 2;
            if (Greater()(this->at(last_index), this->at(parent_index)))
            {
                std::swap(this->at(parent_index), this->at(last_index));
            }
            
            last_index = parent_index;
        }
    }
};

int main(int argc, const char * argv[])
{
    heap<int> heap = {8, 5, 3, 2, 1};
//    heap<int> heap = {1, 2, 3, 5 ,8};
//    heap<int> heap = {5, 2, 8, 1 ,3};
//    heap.popTop();
//    heap.push(10);
    
    return 0;
}
