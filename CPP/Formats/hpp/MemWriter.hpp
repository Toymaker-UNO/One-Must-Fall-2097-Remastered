#ifndef OPENOMF_FORMATS_MEMWRITER_HPP
#define OPENOMF_FORMATS_MEMWRITER_HPP

#include "../Utils/hpp/Allocator.hpp"
#include "Writer.hpp"
#include <cstdint>
#include <cstring>

namespace openomf {
namespace formats {

/** C formats/internal/memwriter 대응. 메모리 버퍼에 쓰기. */
class MemWriter {
public:
    static constexpr size_t GROW = 64;

    MemWriter() : m_buf(nullptr), m_real_len(0), m_data_len(0) {}

    static MemWriter* open() {
        MemWriter* w = static_cast<MemWriter*>(omf_calloc(1, sizeof(MemWriter)));
        w->m_buf = static_cast<char*>(omf_calloc(GROW, 1));
        w->m_real_len = GROW;
        w->m_data_len = 0;
        return w;
    }
    void save(Writer* a_dst) const {
        a_dst->write_buf(m_buf, static_cast<size_t>(m_data_len));
    }
    void close() {
        if (m_buf) omf_free(m_buf);
        m_buf = nullptr;
        openomf::utils::Allocator::free_real(this);
    }
    long pos() const { return m_data_len; }
    void xor_byte(uint8_t a_key) {
        for (long k = 0; k < m_data_len; k++)
            m_buf[k] = static_cast<char>(static_cast<uint8_t>(a_key + static_cast<uint8_t>(k)) ^ static_cast<uint8_t>(m_buf[k]));
    }
    void write_buf(const char* a_buf, int a_len) {
        if (static_cast<long>(m_real_len) < m_data_len + a_len) {
            size_t need = static_cast<size_t>(m_data_len + a_len + GROW);
            m_buf = static_cast<char*>(omf_realloc(m_buf, need));
            m_real_len = need;
        }
        std::memcpy(m_buf + m_data_len, a_buf, static_cast<size_t>(a_len));
        m_data_len += a_len;
    }
    void write_ubyte(uint8_t a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 1); }
    void write_uword(uint16_t a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 2); }
    void write_udword(uint32_t a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 4); }
    void write_byte(int8_t a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 1); }
    void write_word(int16_t a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 2); }
    void write_dword(int32_t a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 4); }
    void write_float(float a_v) { write_buf(reinterpret_cast<const char*>(&a_v), 4); }
    void write_fill(char a_content, int a_len) {
        if (static_cast<long>(m_real_len) < m_data_len + a_len) {
            size_t need = static_cast<size_t>(m_data_len + a_len + GROW);
            m_buf = static_cast<char*>(omf_realloc(m_buf, need));
            m_real_len = need;
        }
        std::memset(m_buf + m_data_len, a_content, static_cast<size_t>(a_len));
        m_data_len += a_len;
    }

private:
    char* m_buf;
    long m_real_len;
    long m_data_len;
};

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_MEMWRITER_HPP
