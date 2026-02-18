#ifndef OPENOMF_UTILS_VECTOR_HPP
#define OPENOMF_UTILS_VECTOR_HPP

#include "Iterator.hpp"
#include "Allocator.hpp"
#include "MiscMath.hpp"
#include <cstddef>
#include <cstring>
#include <cstdlib>

namespace openomf {
namespace utils {

using vector_free_cb = void (*)(void*);
using vector_compare_func = int (*)(const void*, const void*);

/** C utils/vector 대응: 블록 단위 가변 버퍼. */
class Vector {
public:
    Vector() = default;
    ~Vector() { vector_free(); }

    void vector_create(unsigned int a_block_size);
    void vector_create_cb(unsigned int a_block_size, vector_free_cb a_free_cb);
    void vector_create_with_size(unsigned int a_block_size, unsigned int a_initial_size);
    void vector_create_with_size_cb(unsigned int a_block_size, unsigned int a_initial_size,
                                   vector_free_cb a_free_cb);
    void vector_clone(const Vector& a_src);
    void vector_free();

    void* vector_get(unsigned int a_key) const;
    int vector_set(unsigned int a_key, const void* a_value);
    void vector_append(const void* a_value);
    void* vector_append_ptr();

    void vector_sort(vector_compare_func a_cf);
    int vector_delete_at(unsigned int a_index);
    int vector_swapdelete_at(unsigned int a_index);
    int vector_delete(Iterator& a_iter);
    void vector_pop();
    void* vector_back() const;
    void vector_iter_begin(Iterator& a_iter) const;
    void vector_iter_end(Iterator& a_iter) const;
    void vector_clear();

    unsigned int vector_size() const { return m_blocks; }
    /** C vector.data 대응: 내부 버퍼 포인터. */
    char* data() { return m_data; }
    const char* data() const { return m_data; }

private:
    void vector_init();
    void vector_grow();

    char* m_data = nullptr;
    unsigned int m_block_size = 0;
    unsigned int m_blocks = 0;
    unsigned int m_reserved = 0;
    vector_free_cb m_free_cb = nullptr;
};

inline void Vector::vector_init()
{
    m_blocks = 0;
    m_free_cb = nullptr;
    if (m_reserved) {
        m_data = static_cast<char*>(omf_malloc(m_reserved * m_block_size));
    } else {
        m_data = nullptr;
    }
}

inline void Vector::vector_create(unsigned int a_block_size)
{
    m_block_size = a_block_size;
    m_reserved = 32;
    vector_init();
}

inline void Vector::vector_create_cb(unsigned int a_block_size, vector_free_cb a_free_cb)
{
    vector_create(a_block_size);
    m_free_cb = a_free_cb;
}

inline void Vector::vector_create_with_size(unsigned int a_block_size, unsigned int a_initial_size)
{
    m_block_size = a_block_size;
    m_reserved = a_initial_size;
    vector_init();
}

inline void Vector::vector_create_with_size_cb(unsigned int a_block_size, unsigned int a_initial_size,
                                               vector_free_cb a_free_cb)
{
    vector_create_with_size(a_block_size, a_initial_size);
    m_free_cb = a_free_cb;
}

inline void Vector::vector_clone(const Vector& a_src)
{
    m_block_size = a_src.m_block_size;
    m_blocks = a_src.m_blocks;
    m_reserved = a_src.m_reserved;
    m_free_cb = a_src.m_free_cb;
    size_t len = m_reserved * m_block_size;
    m_data = static_cast<char*>(omf_malloc(len));
    std::memcpy(m_data, a_src.m_data, len);
}

inline void Vector::vector_clear()
{
    if (m_free_cb != nullptr) {
        for (unsigned int i = 0; i < m_blocks; ++i) {
            m_free_cb(m_data + m_block_size * i);
        }
    }
    m_blocks = 0;
}

inline void Vector::vector_free()
{
    vector_clear();
    m_reserved = 0;
    m_block_size = 0;
    omf_free(m_data);
    m_data = nullptr;
}

inline void* Vector::vector_get(unsigned int a_key) const
{
    if (a_key >= m_blocks) {
        return nullptr;
    }
    return m_data + m_block_size * a_key;
}

inline void* Vector::vector_back() const
{
    if (m_blocks == 0) {
        return nullptr;
    }
    return m_data + m_block_size * (m_blocks - 1);
}

inline int Vector::vector_set(unsigned int a_key, const void* a_value)
{
    if (a_key >= m_blocks) {
        return 1;
    }
    void* dst = m_data + a_key * m_block_size;
    std::memmove(dst, a_value, m_block_size);
    return 0;
}

inline void Vector::vector_grow()
{
    int current = MiscMath::max2(1, static_cast<int>(m_reserved));
    int new_size = current + MiscMath::max2(1, (current >> 2));
    m_data = static_cast<char*>(omf_realloc(m_data, new_size * m_block_size));
    m_reserved = static_cast<unsigned int>(new_size);
}

inline void* Vector::vector_append_ptr()
{
    if (m_blocks >= m_reserved) {
        vector_grow();
    }
    void* dst = m_data + m_blocks * m_block_size;
    ++m_blocks;
    return dst;
}

inline void Vector::vector_append(const void* a_value)
{
    std::memmove(vector_append_ptr(), a_value, m_block_size);
}

inline void Vector::vector_pop()
{
    if (m_blocks > 0) {
        --m_blocks;
    }
}

inline int Vector::vector_delete_at(unsigned int a_index)
{
    if (m_blocks == 0) {
        return 1;
    }
    if (a_index + 1 < m_blocks) {
        void* dst = m_data + a_index * m_block_size;
        void* src = m_data + (a_index + 1) * m_block_size;
        unsigned int size = (m_blocks - 1 - a_index) * m_block_size;
        std::memmove(dst, src, size);
    }
    --m_blocks;
    return 0;
}

inline int Vector::vector_swapdelete_at(unsigned int a_index)
{
    if (m_blocks == 0) {
        return 1;
    }
    unsigned int last = m_blocks - 1;
    if (a_index != last) {
        std::memcpy(vector_get(a_index), vector_get(last), m_block_size);
    }
    --m_blocks;
    return 0;
}

inline int Vector::vector_delete(Iterator& a_iter)
{
    if (m_blocks == 0) {
        return 1;
    }
    int real = a_iter.is_forward() ? a_iter.inow() - 1 : a_iter.inow() + 1;
    if (real < 0) {
        real = 0;
    }
    if (real + 1 < static_cast<int>(m_blocks)) {
        void* dst = m_data + real * m_block_size;
        void* src = m_data + (real + 1) * m_block_size;
        unsigned int size = (m_blocks - 1 - static_cast<unsigned int>(real)) * m_block_size;
        std::memmove(dst, src, size);
        if (a_iter.is_forward()) {
            a_iter.set_inow(a_iter.inow() - 1);
        }
    }
    if (m_blocks > 0) {
        --m_blocks;
    }
    return 0;
}

inline void Vector::vector_sort(vector_compare_func a_cf)
{
    std::qsort(m_data, m_blocks, m_block_size, a_cf);
}

inline void Vector::vector_iter_begin(Iterator& a_iter) const
{
    const Vector* vec = this;
    a_iter.set_data(vec);
    a_iter.set_inow(0);
    a_iter.set_ended(m_blocks == 0);
    a_iter.set_forward(true);
    a_iter.set_next([vec, &a_iter]() {
        if (a_iter.inow() + 1 >= static_cast<int>(vec->m_blocks)) {
            a_iter.set_ended(true);
        }
        void* addr = vec->m_data + a_iter.inow() * vec->m_block_size;
        a_iter.set_inow(a_iter.inow() + 1);
        return addr;
    });
    a_iter.set_prev(nullptr);
    a_iter.set_peek(nullptr);
}

inline void Vector::vector_iter_end(Iterator& a_iter) const
{
    const Vector* vec = this;
    a_iter.set_data(vec);
    a_iter.set_inow(static_cast<int>(m_blocks) - 1);
    a_iter.set_ended(m_blocks == 0);
    a_iter.set_forward(false);
    a_iter.set_next(nullptr);
    a_iter.set_prev([vec, &a_iter]() {
        if (a_iter.inow() == 0) {
            a_iter.set_ended(true);
        }
        void* addr = vec->m_data + a_iter.inow() * vec->m_block_size;
        a_iter.set_inow(a_iter.inow() - 1);
        return addr;
    });
    a_iter.set_peek(nullptr);
}

}  // namespace utils
}  // namespace openomf

#endif
