
#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <iterator>
#include <iostream>
#include <algorithm>
#include <utility>


using namespace std;
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity) : capacity_(capacity) {};
    size_t capacity_ = 0;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    SimpleVector(size_t size) : arr(size)
    {
        size_ = size;
        capacity_ = size;
        //fill(begin(), end(), Type());
        generate(arr.Get(), arr.Get() + size_, [] { return Type(); });
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : arr(size)
    {
        // Напишите тело конструктора самостоятельно        
        size_ = size;
        capacity_ = size;
        fill(begin(), end(), value);
    }

    //Конструктор копирования.
    SimpleVector(const SimpleVector<Type>& other) : arr(other.size_)
    {
        // Напишите тело конструктора самостоятельно
        size_ = other.size_;
        capacity_ = other.capacity_;
        copy(other.begin(), other.end(), begin());
    }
    //Конструктор перемещения.
    SimpleVector(SimpleVector<Type>&& other) : arr(other.size_)
    {
        // Напишите тело конструктора самостоятельно
        swap(other);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(const initializer_list<Type> init) : arr(init.size())
    {
        // Напишите тело конструктора самостоятельно
        size_ = init.size();
        capacity_ = init.size();
        copy(init.begin(), init.end(), begin());
    }

    SimpleVector(ReserveProxyObj capacity)  
    {
       Reserve(capacity.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept
    {
        // Напишите тело самостоятельно
        assert(index < size_);
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept
    {
        // Напишите тело самостоятельно
        assert(index < size_);
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index)
    {
        // Напишите тело самостоятельно
        if (size_ <= index)
        {
            throw out_of_range(" out_of_range!");
        }
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const
    {
        // Напишите тело самостоятельно
        if (size_ <= index)
        {
            throw out_of_range(" out_of_range!");
        }
        return arr[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept
    {
        // Напишите тело самостоятельно
        size_ = 0;
    }

    // Изменяет размер массива.
    void Resize(size_t new_size)
    {      
        Reserve(new_size);
        for (size_t i = size_; i < new_size; ++i) {
            arr[i] = Type();
        }
        size_ = new_size; 
    }
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item)
    {
        Insert(end(), item);
    }
    void PushBack(Type&& item)
    {            
        Insert(end(), move(item));
    }


    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept
    {
        // Напишите тело самостоятельно
        assert(size_ > 0);
        --size_;
    }
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept
    {
        // Напишите тело самостоятельно
        arr.swap(other.arr);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    //Reserve задает ёмкость вектора
    void Reserve(size_t new_capacity)
    {
        if (new_capacity > capacity_)
        {
            ArrayPtr<Type> tmp(new_capacity);
            if (size_ > 0)
            {
                for (size_t i = 0; i < size_; i++)
                {
                    tmp[i] = move(arr[i]);
                }
            }
            arr.swap(tmp);
            capacity_ = new_capacity;
        }
    };

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1

    Iterator Insert(ConstIterator pos, const Type& value)
    {
        // Напишите тело самостоятельно
        assert(size_t(distance(begin(), (Iterator)pos)) <= size_);

        if (size_ == 0) { Resize(1); *begin() = value; return begin(); }
        if (size_ < capacity_)
        {
            copy_backward(pos, cend(), end() + 1);
            auto dist = distance(cbegin(), pos);
            arr[dist] = value;
            ++size_;
            return (begin() + dist);
        }
        else if (size_ >= capacity_)
        {
            auto dist = distance(cbegin(), pos);
            SimpleVector<Type> tmp(size_ * 2);
            copy(cbegin(), pos, tmp.begin());
            copy_backward(pos, cend(), (tmp.end() - (size_ - 1)));
            tmp[dist] = value;
            size_t size_tmp = size_;
            swap(tmp);
            size_ = ++size_tmp;
            return (begin() + dist);
        }
        return{};
    }
    Iterator Insert(ConstIterator pos,  Type&& value)
    {
         //Напишите тело самостоятельно
        assert(size_t(distance(begin(), (Iterator)pos)) <= size_);
        
        if (size_ == 0) { Resize(1); *begin() = move(value); return begin(); }
        if (size_ < capacity_)
        {
            auto dist = distance((Iterator)pos, end());
            SimpleVector<Type> tmp(dist);
            move((Iterator)pos, end(), tmp.begin());
            move(tmp.begin(),tmp.end(), (Iterator)pos+1);
            ++size_;
            *(Iterator)pos = move(value);
            return (Iterator)pos;
        }
        auto dist = distance(begin(), (Iterator)pos);
        SimpleVector<Type> tmp(size_ * 2);
        move(begin(), (Iterator)pos, tmp.begin());
        move((Iterator)pos, end(), tmp.begin()+ dist);
        tmp[dist] = move(value);
        size_t size_tmp = size_;
        swap(tmp);
        size_ = ++size_tmp;      
        return (begin() + dist);
    }
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos)
    {
        // Напишите тело самостоятельно
        assert(size_>0);
        move(next(Iterator(pos)), end(), Iterator(pos)); 
        --size_;
        return Iterator(pos);
    }
    SimpleVector& operator=(const SimpleVector& rhs)
    {
        // Напишите тело конструктора самостоятельно
        if (&rhs.arr == &arr) { return *this; }
        //случай с самоприсваиванием

        SimpleVector<Type> tmp(rhs.size_);
        copy(rhs.begin(), rhs.end(), tmp.begin());
        swap(tmp);
        return *this;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept
    {
        // Напишите тело самостоятельно
        return Iterator{ arr.Get() };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept
    {
        // Напишите тело самостоятельно
        return Iterator{ arr.Get() + size_ };
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept
    {
        // Напишите тело самостоятельно
        return ConstIterator{ arr.Get() };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept
    {
        // Напишите тело самостоятельно
        return ConstIterator{ arr.Get() + size_ };
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept
    {
        // Напишите тело самостоятельно
        return ConstIterator{ arr.Get() };
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept
    {
        // Напишите тело самостоятельно
        return ConstIterator{ arr.Get() + size_ };
    }

private:
    ArrayPtr<Type> arr;
    size_t size_ = 0;
    size_t capacity_ = 0;

};
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    if (lhs.GetSize() != rhs.GetSize()) { return false; }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}
template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return !(lhs == rhs);
}
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    // Заглушка. Напишите тело самостоятельно
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    // Заглушка. Напишите тело самостоятельно
    return (lhs == rhs || lhs < rhs);
}
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    // Заглушка. Напишите тело самостоятельно
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    // Заглушка. Напишите тело самостоятельно
    return (lhs == rhs || !(lhs < rhs));
}
