#ifndef OPENOMF_UTILS_ITERATOR_HPP
#define OPENOMF_UTILS_ITERATOR_HPP

#include <functional>

namespace openomf {
namespace utils {

/** C utils/iterator 대응: 제네릭 순회 (next/prev/peek). 복사 불가. */
class Iterator {
public:
    using NextFn = std::function<void*()>;
    using PrevFn = std::function<void*()>;
    using PeekFn = std::function<void*()>;

    Iterator() = default;
    Iterator(const Iterator&) = delete;
    Iterator& operator=(const Iterator&) = delete;

    void set_data(const void* a_data) { m_data = a_data; }
    void set_inow(int a_inow) { m_inow = a_inow; }
    void set_ended(bool a_ended) { m_ended = a_ended; }
    void set_next(NextFn a_next) { m_next = std::move(a_next); }
    void set_prev(PrevFn a_prev) { m_prev = std::move(a_prev); }
    void set_peek(PeekFn a_peek) { m_peek = std::move(a_peek); }

    const void* data() const { return m_data; }
    int inow() const { return m_inow; }
    bool ended() const { return m_ended; }
    void set_forward(bool a_forward) { m_forward = a_forward; }
    bool is_forward() const { return m_forward; }

    void* iter_next() {
        if (!m_next) return nullptr;
        return m_next();
    }

    void* iter_prev() {
        if (!m_prev) return nullptr;
        return m_prev();
    }

    void* iter_peek() {
        if (!m_peek) return nullptr;
        return m_peek();
    }

private:
    const void* m_data = nullptr;
    int m_inow = 0;
    bool m_ended = false;
    bool m_forward = true;
    NextFn m_next;
    PrevFn m_prev;
    PeekFn m_peek;
};

}  // namespace utils
}  // namespace openomf

#endif
