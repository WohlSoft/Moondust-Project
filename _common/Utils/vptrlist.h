/*
VPtrList - implementation of PtrList based on std::vector<std::shared_ptr<T>> inherence
           implements same API as PtrList, but works faster and has things as std::vector has

Copyright (c) 2017 Vitaliy Novichkov <admin@wohlnet.ru>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef VPTRLIST_H
#define VPTRLIST_H

#include <vector>
#include <memory>
#include <assert.h>

template<class T>
class VPtrList : public std::vector<std::unique_ptr<T>>
{
public:
    typedef std::unique_ptr<T> SHptr;
    typedef std::vector<SHptr> vecPTR;
    typedef typename vecPTR::iterator       S_iterator;
    typedef typename vecPTR::const_iterator S_const_iterator;
    typedef typename vecPTR::reverse_iterator       SR_iterator;
    typedef typename vecPTR::const_reverse_iterator SR_const_iterator;

    typedef T                               value_type;
    typedef typename vecPTR::allocator_type allocator_type;
    typedef size_t                          size_type;
    typedef std::ptrdiff_t                  difference_type;
    typedef T&                              reference;
    typedef const T&                        const_reference;
    typedef typename vecPTR::pointer        pointer;

    template<typename TT, class SIterator>
    class VPtrIterator : public SIterator
    {
    public:
        VPtrIterator(const SIterator &o) : SIterator(o) {}
        virtual ~VPtrIterator() {}

        TT &operator*()
        {
            return *(SIterator::operator*());
        }

        TT *operator->()
        {
            return (SIterator::operator*().get());
        }

        TT &operator[](size_t index)
        {
            return *(*this + index);
        }

        friend VPtrIterator operator+(const VPtrIterator &it, int inc)
        {
            S_iterator i = it;
            return iterator(i += inc);
        }
        friend VPtrIterator operator-(const VPtrIterator &it, int inc)
        {
            S_iterator i = it;
            return iterator(i -= inc);
        }
        friend VPtrIterator operator+(const VPtrIterator &it1, const VPtrIterator &it2)
        {
            return iterator(it1 + it2);
        }
        friend VPtrIterator operator-(const VPtrIterator &it1, const VPtrIterator &it2)
        {
            return iterator(it1 - it2);
        }
        VPtrIterator &operator++()
        {
            SIterator::operator++();
            return *this;
        }
        VPtrIterator &operator++(int)
        {
            SIterator::operator++();
            return *this;
        }
        VPtrIterator &operator+=(int inc)
        {
            SIterator::operator+=(inc);
            return *this;
        }
        VPtrIterator &operator-=(int dec)
        {
            SIterator::operator-=(dec);
            return *this;
        }
        VPtrIterator &operator+=(const VPtrIterator &inc)
        {
            SIterator::operator+=(inc);
            return *this;
        }
        VPtrIterator &operator-=(const VPtrIterator &dec)
        {
            SIterator::operator-=(dec);
            return *this;
        }
    };

    typedef VPtrIterator<T, S_iterator>                 iterator;
    typedef VPtrIterator<const T, S_const_iterator>     const_iterator;

    typedef VPtrIterator<T, SR_iterator>                reverse_iterator;
    typedef VPtrIterator<const T, SR_const_iterator>    const_reverse_iterator;

    VPtrList() : vecPTR()
    {}

    VPtrList(const VPtrList<T>& o) : vecPTR()
    {
        if(this != &o)
        {
            vecPTR::clear();
            this->append(o);
        }
    }

    VPtrList(std::initializer_list<T> il) : vecPTR()
    {
        this->assign(il);
    }

    VPtrList(size_t size) : vecPTR(size)
    {}

    VPtrList &operator=(const VPtrList &o)
    {
        if(this != &o)
        {
            vecPTR::clear();
            this->append(o);
        }
        return *this;
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        this->reserve(std::distance(first, last));
        while(first < last)
            vecPTR::push_back(SHptr(new T(*(first++))));
    }

    void assign(size_t n, const T& val)
    {
        this->reserve(n);
        while((n--) > 0 )
            vecPTR::push_back(SHptr(new T(val)));
    }

    void assign(std::initializer_list<T> il)
    {
        this->reserve(std::distance(il.begin(), il.end()));
        auto i = il.begin();
        while(i != il.end())
            vecPTR::push_back(SHptr(new T(*(i++))));
    }

    iterator begin()
    {
        return vecPTR::begin();
    }
    iterator end()
    {
        return vecPTR::end();
    }
    const_iterator begin() const
    {
        return vecPTR::begin();
    }
    const_iterator end() const
    {
        return vecPTR::end();
    }

    const_iterator cbegin() const
    {
        return vecPTR::cbegin();
    }
    const_iterator cend() const
    {
        return vecPTR::cend();
    }

    reverse_iterator rbegin()
    {
        return vecPTR::begin();
    }
    reverse_iterator rend()
    {
        return vecPTR::rend();
    }
    const_reverse_iterator rbegin() const
    {
        return vecPTR::rbegin();
    }
    const_reverse_iterator rend() const
    {
        return vecPTR::rend();
    }

    const_reverse_iterator crbegin() const
    {
        return vecPTR::crbegin();
    }
    const_reverse_iterator crend() const
    {
        return vecPTR::crend();
    }

    size_t count() const
    {
        return vecPTR::size();
    }

    bool contains(const T &item) const
    {
        return indexOf(item) >= 0;
    }

    ssize_t indexOf(const T &item) const
    {
        size_t s        = vecPTR::size();
        const SHptr *d  = vecPTR::data();
        size_t i = 0;
        for(; i < s; i++)
        {
            if(*d[i] == item)
                return ssize_t(i);
        }
        return -1;
    }

    ssize_t lastIndexOf(const T &item) const
    {
        ssize_t     s   = vecPTR::size();
        const SHptr *d  = vecPTR::data();
        ssize_t     i   = s - 1;
        for(; i >= 0; i--)
        {
            if(*d[i] == item)
                return ssize_t(i);
        }
        return -1;
    }

    iterator find(const T &item)
    {
        return this->find(item, this->begin());
    }

    iterator find(const T &item, iterator beg)
    {
        iterator i = beg;
        for(; i != end(); i++)
        {
            if(*i == item)
                break;
        }
        return i;
    }

    const_iterator find(const T &item) const
    {
        return this->find(item, this->cbegin());
    }

    const_iterator find(const T &item, const_iterator beg) const
    {
        const_iterator i = beg;
        for(; i != end(); i++)
        {
            if(*i == item)
                break;
        }
        return i;
    }

    iterator find_last_of(const T &item)
    {
        return this->find_last_of(item, this->rbegin());
    }

    iterator find_last_of(const T &item, reverse_iterator beg)
    {
        reverse_iterator i = beg;
        for(; i != end(); i++)
        {
            if(*i == item)
                break;
        }
        return i;
    }

    const_iterator find_last_of(const T &item) const
    {
        return this->find_last_of(item, this->crbegin());
    }

    const_iterator find_last_of(const T &item, const_reverse_iterator beg) const
    {
        const_reverse_iterator i = beg;
        for(; i != end(); i++)
        {
            if(*i == item)
                break;
        }
        return i;
    }

    void removeOne(const T &item)
    {
        iterator i = this->begin();
        for(; i != end(); i++)
        {
            if(*i == item)
            {
                vecPTR::erase(i);
                break;
            }
        }
    }

    void removeAll(const T &item)
    {
        iterator i = this->begin();
        for(; i != end();)
        {
            if(*i == item)
                i == vecPTR::erase(i);
            else
                i++;
        }
    }

    iterator erase(iterator pos)
    {
        assert(pos < end());
        return iterator(vecPTR::erase(pos));
    }

    iterator erase(iterator from, iterator to)
    {
        assert(from < end());
        assert(to < end());
        assert(from <= to);
        if(from == to)
            return from;
        return iterator(vecPTR::erase(from, to));
    }

    void removeAt(size_t at)
    {
        vecPTR::erase(begin() + at);
    }

    void removeAt(size_t at, size_t num)
    {
        vecPTR::erase(begin() + at, begin() + (at + num));
    }

    void pop_front()
    {
        vecPTR::erase(begin(), begin() + 1);
    }

    void swap(size_t from, size_t to)
    {
        if(from == to)
            return;
        std::swap(*(vecPTR::begin() + from), *(vecPTR::begin() + to));
    }

    void move(size_t from, size_t to)
    {
        size_t m_size = vecPTR::size();
        SHptr *m_data = vecPTR::data();
        assert(m_size > from);
        assert(m_size > to);
        if(from == to)
            return;
        if(from < to)
        {
            SHptr it = std::move(m_data[from]);
            while(from < to)
            {
                m_data[from] = std::move(m_data[from + 1]);
                from++;
            }
            m_data[to] = std::move(it);
        }
        else
        {
            SHptr it = std::move(m_data[from]);
            while(from > to)
            {
                m_data[from] = std::move(m_data[from - 1]);
                from--;
            }
            m_data[to] = std::move(it);
        }
    }

    void push_back(const T &item)
    {
        vecPTR::push_back(SHptr(new T(item)));
    }

    void push_back(T &&item)
    {
        vecPTR::emplace_back(SHptr(new T(std::move(item))));
    }

    template<typename... _Args>
    void emplace_back(_Args&&... __args)
    {
        vecPTR::emplace_back(std::move(SHptr(new T(std::forward<_Args>(__args)...))));
    }

    void push_front(const T &item)
    {
        vecPTR::insert(begin(), SHptr(new T(item)));
    }

    void push_front(T &&item)
    {
        vecPTR::insert(begin(), SHptr(new T(std::move(item))));
    }

    template<typename... _Args>
    iterator emplace(const_iterator pos, _Args&&... __args)
    {
        return vecPTR::emplace(pos, std::move(SHptr(new T(std::forward<_Args>(__args)...))));
    }

    void append(const T &item)
    {
        vecPTR::push_back(SHptr(new T(item)));
    }

    void append(const VPtrList<T> &array)
    {
        vecPTR::reserve(array.size());
        for(const T &t : array)
            vecPTR::push_back(SHptr(new T(t)));
    }

    iterator insert(size_t at, const T &item)
    {
        return iterator(vecPTR::insert(begin() + at, SHptr(new T(item))));
    }

    iterator insert(const_iterator pos, const T &item)
    {
        iterator(vecPTR::insert(pos, SHptr(new T(item))));
    }

    iterator insert(const_iterator pos, T &&item)
    {
        iterator(vecPTR::insert(pos, SHptr(new T(std::move(item)))));
    }

    T &last()
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::back();
    }

    T &first()
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::front();
    }

    T &back()
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::back();
    }

    T &front()
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::front();
    }

    const T &last() const
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::back();
    }

    const T &first() const
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::front();
    }

    const T &back() const
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::back();
    }

    const T &front() const
    {
        assert(vecPTR::size() > 0);
        return *vecPTR::front();
    }

    T &at(unsigned int index)
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    T &at(int index)
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    T &at(unsigned long index)
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    T &at(long index)
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    T &at(unsigned long long index)
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    T &at(long long index)
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    const T &at(unsigned int index) const
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    const T &at(int index) const
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    const T &at(unsigned long index) const
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    const T &at(long index) const
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    const T &at(unsigned long long index) const
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }

    const T &at(long long index) const
    {
        return *(vecPTR::at(static_cast<size_t>(index)));
    }


    T &operator[](unsigned int index)
    {
        assert(index < static_cast<unsigned int>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    T &operator[](int index)
    {
        assert(index >= 0);
        assert(index < static_cast<int>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    T &operator[](unsigned long index)
    {
        assert(index < static_cast<unsigned long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    T &operator[](long index)
    {
        assert(index >= 0);
        assert(index < static_cast<unsigned long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    T &operator[](unsigned long long index)
    {
        assert(index < static_cast<unsigned long long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    T &operator[](long long index)
    {
        assert(index >= 0);
        assert(index < static_cast<long long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }


    const T &operator[](unsigned int index) const
    {
        assert(index < static_cast<unsigned int>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    const T &operator[](int index) const
    {
        assert(index >= 0);
        assert(index < static_cast<int>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    const T &operator[](unsigned long index) const
    {
        assert(index < static_cast<unsigned long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    const T &operator[](long index) const
    {
        assert(index >= 0);
        assert(index < static_cast<unsigned long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    const T &operator[](unsigned long long index) const
    {
        assert(index < static_cast<unsigned long long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }

    const T &operator[](long long index) const
    {
        assert(index >= 0);
        assert(index < static_cast<long long>(vecPTR::size()));
        return *(vecPTR::at(index));
    }
};

#endif // VPTRLIST_H
