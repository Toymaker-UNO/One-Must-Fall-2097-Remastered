#ifndef OPENOMF_FORMATS_MEMREADER_HPP
#define OPENOMF_FORMATS_MEMREADER_HPP

#include "../Utils/hpp/Allocator.hpp"
#include "Reader.hpp"
#include <cstdint>
#include <cstring>
#include <cassert>

namespace openomf {
namespace formats {

/** C formats/internal/memreader 대응. 메모리 버퍼에서 읽기. */
class MemReader {
public:
    MemReader() : m_buf(nullptr), m_owned(0), m_len(0), m_pos(0) {}

    /** buf는 MemReader가 소유하지 않음. owned=0. */
    static MemReader* open(char* a_buf, long a_len) {
        MemReader* r = static_cast<MemReader*>(omf_calloc(1, sizeof(MemReader)));
        r->m_buf = a_buf;
        r->m_len = a_len;
        r->m_pos = 0;
        r->m_owned = 0;
        return r;
    }
    /** reader에서 len바이트 읽어서 소유한 버퍼로 보관. owned=1. */
    static MemReader* open_from_reader(Reader* a_reader, int a_len) {
        if (a_len == 0) return open(nullptr, 0);
        char* buf = static_cast<char*>(omf_calloc(1, static_cast<size_t>(a_len)));
        a_reader->read_buf(buf, static_cast<size_t>(a_len));
        MemReader* r = open(buf, a_len);
        r->m_owned = 1;
        return r;
    }
    void close() {
        if (m_owned && m_buf) omf_free(m_buf);
        m_buf = nullptr;
        openomf::utils::Allocator::free_real(this);
    }
    long size() const { return m_len; }
    long pos() const { return m_pos; }
    void xor_byte(uint8_t a_key) {
        for (long k = 0; k < m_len; k++)
            m_buf[k] = static_cast<char>(static_cast<uint8_t>(a_key + static_cast<uint8_t>(k)) ^ static_cast<uint8_t>(m_buf[k]));
    }
    int read_buf(char* a_out, int a_len) {
        assert(m_buf != nullptr && a_out != nullptr);
        if (m_pos + a_len > m_len) return 0;
        std::memcpy(a_out, m_buf + m_pos, static_cast<size_t>(a_len));
        m_pos += a_len;
        return 1;
    }
    uint8_t read_ubyte() { uint8_t r = 0; read_buf(reinterpret_cast<char*>(&r), 1); return r; }
    uint16_t read_uword() { uint16_t r = 0; read_buf(reinterpret_cast<char*>(&r), 2); return r; }
    uint32_t read_udword() { uint32_t r = 0; read_buf(reinterpret_cast<char*>(&r), 4); return r; }
    int8_t read_byte() { int8_t r = 0; read_buf(reinterpret_cast<char*>(&r), 1); return r; }
    int16_t read_word() { int16_t r = 0; read_buf(reinterpret_cast<char*>(&r), 2); return r; }
    int32_t read_dword() { int32_t r = 0; read_buf(reinterpret_cast<char*>(&r), 4); return r; }
    float read_float() { float r = 0; read_buf(reinterpret_cast<char*>(&r), 4); return r; }
    void skip(unsigned int a_nbytes) { m_pos += a_nbytes; }

private:
    char* m_buf;
    int m_owned;
    long m_len;
    long m_pos;
};

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_MEMREADER_HPP
