#ifndef OPENOMF_UTILS_ARRAY_HPP
#define OPENOMF_UTILS_ARRAY_HPP

#include "Iterator.hpp"
#include "Allocator.hpp"
#include <cstddef>

namespace openomf {
namespace utils {

/** C utils/array 대응: void* 인덱스 배열. */
class Array {
public:
    static const unsigned ARRAY_START_SIZE = 64;

    Array() = default;
    ~Array() { array_free(); }

    void array_create();
    void array_free();
    void array_set(unsigned int a_key, const void* a_ptr);
    void* array_get(unsigned int a_key) const;

    void array_iter_begin(Iterator& a_iter) const;
    void array_iter_end(Iterator& a_iter) const;

    unsigned int allocated_size() const { return m_allocated_size; }
    unsigned int filled() const { return m_filled; }

private:
    unsigned int m_allocated_size = 0;
    unsigned int m_filled = 0;
    void** m_data = nullptr;
};

namespace detail {

inline void array_nullify(void** a_ptr, unsigned int a_len)
{
    for (unsigned int i = 0; i < a_len; ++i) {
        a_ptr[i] = nullptr;
    }
}

inline void* array_iter_next(Iterator* a_iter, const Array* a_arr)
{
    void* out = nullptr;
    while (a_iter->inow() < static_cast<int>(a_arr->allocated_size())) {
        void* p = a_arr->array_get(static_cast<unsigned int>(a_iter->inow()));
        if (p != nullptr) {
            out = p;
            a_iter->set_inow(a_iter->inow() + 1);
            break;
        }
        a_iter->set_inow(a_iter->inow() + 1);
    }
    if (a_iter->inow() + 1 >= static_cast<int>(a_arr->allocated_size())) {
        a_iter->set_ended(true);
    }
    return out;
}

inline void* array_iter_prev(Iterator* a_iter, const Array* a_arr)
{
    void* out = nullptr;
    if (a_iter->inow() == 0) {
        a_iter->set_ended(true);
        return nullptr;
    }
    while (a_iter->inow() >= 0) {
        void* p = a_arr->array_get(static_cast<unsigned int>(a_iter->inow()));
        if (p != nullptr) {
            out = p;
            a_iter->set_inow(a_iter->inow() - 1);
            break;
        }
        a_iter->set_inow(a_iter->inow() - 1);
    }
    return out;
}

}  // namespace detail

inline void Array::array_create()
{
    m_allocated_size = ARRAY_START_SIZE;
    m_filled = 0;
    m_data = static_cast<void**>(omf_calloc(1, m_allocated_size * sizeof(void*)));
    detail::array_nullify(m_data, m_allocated_size);
}

inline void Array::array_free()
{
    m_allocated_size = 0;
    m_filled = 0;
    omf_free(m_data);
}

inline void Array::array_set(unsigned int a_key, const void* a_ptr)
{
    if (a_key >= m_allocated_size) {
        unsigned int new_size = m_allocated_size * 2;
        if (a_key > new_size) {
            new_size = a_key;
        }
        m_data = static_cast<void**>(omf_realloc(m_data, new_size * sizeof(void*)));
        detail::array_nullify(m_data + m_allocated_size, new_size - m_allocated_size);
        m_allocated_size = new_size;
    }
    m_data[a_key] = const_cast<void*>(a_ptr);
    if (a_ptr != nullptr) {
        ++m_filled;
    }
}

inline void* Array::array_get(unsigned int a_key) const
{
    if (a_key >= m_allocated_size) {
        return nullptr;
    }
    return m_data[a_key];
}

inline void Array::array_iter_begin(Iterator& a_iter) const
{
    a_iter.set_data(this);
    a_iter.set_inow(0);
    a_iter.set_ended(m_filled == 0);
    a_iter.set_forward(true);
    a_iter.set_next([this, &a_iter]() { return detail::array_iter_next(&a_iter, this); });
    a_iter.set_prev(nullptr);
    a_iter.set_peek(nullptr);
}

inline void Array::array_iter_end(Iterator& a_iter) const
{
    a_iter.set_data(this);
    a_iter.set_inow(static_cast<int>(m_allocated_size) - 1);
    a_iter.set_ended(m_filled == 0);
    a_iter.set_forward(false);
    a_iter.set_next(nullptr);
    a_iter.set_prev([this, &a_iter]() { return detail::array_iter_prev(&a_iter, this); });
    a_iter.set_peek(nullptr);
}

}  // namespace utils
}  // namespace openomf

#endif
