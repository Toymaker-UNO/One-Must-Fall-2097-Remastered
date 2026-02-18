#ifndef OPENOMF_FORMATS_READER_HPP
#define OPENOMF_FORMATS_READER_HPP

#include "../Utils/hpp/Allocator.hpp"
#include "../Utils/hpp/Io.hpp"
#include "../Utils/hpp/Str.hpp"
#include <cstdint>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdarg>

namespace openomf {
namespace formats {

/** C formats/internal/reader (sd_reader) 대응. 바이너리 파일 읽기. */
class Reader {
public:
    Reader() : m_handle(nullptr), m_filesize(0), m_sd_errno(0) {}

    /** 파일 열기. 실패 시 nullptr 반환. (Windows: 경로 그대로, 비-Windows: scan_directory 미구현 시 경로 그대로) */
    static Reader* open(const char* a_file) {
        FILE* handle = openomf::utils::Io::file_open(a_file, "rb");
        if (!handle) return nullptr;
        if (fseek(handle, 0, SEEK_END) != 0) {
            openomf::utils::Io::file_close(handle);
            return nullptr;
        }
        long filesize = ftell(handle);
        if (filesize == -1) {
            openomf::utils::Io::file_close(handle);
            return nullptr;
        }
        if (fseek(handle, 0, SEEK_SET) != 0) {
            openomf::utils::Io::file_close(handle);
            return nullptr;
        }
        Reader* r = static_cast<Reader*>(omf_calloc(1, sizeof(Reader)));
        r->m_handle = handle;
        r->m_filesize = filesize;
        r->m_sd_errno = 0;
        return r;
    }

    void close() {
        if (m_handle) openomf::utils::Io::file_close(m_handle);
        m_handle = nullptr;
        openomf::utils::Allocator::free_real(this);
    }

    long filesize() const { return m_filesize; }
    int errno_val() const { return m_sd_errno; }
    int set_pos(long a_offset) {
        if (fseek(m_handle, a_offset, SEEK_SET) != 0) {
            m_sd_errno = errno;
            return 0;
        }
        return 1;
    }
    int ok() const { return feof(m_handle) ? 0 : 1; }
    long pos() const {
        long res = ftell(m_handle);
        if (res == -1) const_cast<Reader*>(this)->m_sd_errno = errno;
        return res;
    }

    int read_buf(char* a_buf, size_t a_len) {
        if (fread(a_buf, 1, a_len, m_handle) != a_len) {
            m_sd_errno = ferror(m_handle);
            return 0;
        }
        return 1;
    }
    int peek_buf(char* a_buf, int a_len) {
        if (read_buf(a_buf, static_cast<size_t>(a_len)) == 0) return 0;
        if (fseek(m_handle, ftell(m_handle) - a_len, SEEK_SET) == -1)
            m_sd_errno = errno;
        return 1;
    }

    uint8_t read_ubyte() { uint8_t d = 0; read_buf(reinterpret_cast<char*>(&d), 1); return d; }
    uint16_t read_uword() { uint16_t d = 0; read_buf(reinterpret_cast<char*>(&d), 2); return d; }
    uint32_t read_udword() { uint32_t d = 0; read_buf(reinterpret_cast<char*>(&d), 4); return d; }
    int8_t read_byte() { int8_t d = 0; read_buf(reinterpret_cast<char*>(&d), 1); return d; }
    int16_t read_word() { int16_t d = 0; read_buf(reinterpret_cast<char*>(&d), 2); return d; }
    int32_t read_dword() { int32_t d = 0; read_buf(reinterpret_cast<char*>(&d), 4); return d; }
    float read_float() { float f = 0; read_buf(reinterpret_cast<char*>(&f), 4); return f; }

    uint8_t peek_ubyte() { uint8_t d = 0; peek_buf(reinterpret_cast<char*>(&d), 1); return d; }
    uint16_t peek_uword() { uint16_t d = 0; peek_buf(reinterpret_cast<char*>(&d), 2); return d; }
    uint32_t peek_udword() { uint32_t d = 0; peek_buf(reinterpret_cast<char*>(&d), 4); return d; }
    int8_t peek_byte() { int8_t d = 0; peek_buf(reinterpret_cast<char*>(&d), 1); return d; }
    int16_t peek_word() { int16_t d = 0; peek_buf(reinterpret_cast<char*>(&d), 2); return d; }
    int32_t peek_dword() { int32_t d = 0; peek_buf(reinterpret_cast<char*>(&d), 4); return d; }
    float peek_float() { float f = 0; peek_buf(reinterpret_cast<char*>(&f), 4); return f; }

    int read_scan(const char* a_format, ...) {
        std::va_list argp;
        va_start(argp, a_format);
        int ret = vfscanf(m_handle, a_format, argp);
        va_end(argp);
        return ret;
    }
    int read_line(char* a_buffer, int a_maxlen) {
        return fgets(a_buffer, a_maxlen, m_handle) == nullptr ? 1 : 0;
    }
    int match(const char* a_buf, unsigned int a_nbytes) {
        char* t = static_cast<char*>(omf_malloc(a_nbytes));
        int ok = (peek_buf(t, static_cast<int>(a_nbytes)) != 0 && std::memcmp(t, a_buf, a_nbytes) == 0);
        omf_free(t);
        return ok ? 1 : 0;
    }
    void skip(unsigned int a_nbytes) {
        if (fseek(m_handle, static_cast<long>(a_nbytes), SEEK_CUR) == -1)
            m_sd_errno = errno;
    }
    void read_str(openomf::utils::Str* a_dst) {
        uint16_t len = read_uword();
        if (len > 0) {
            char* buf = static_cast<char*>(omf_calloc(len + 1, 1));
            read_buf(buf, len);
            a_dst->str_from_c(buf);
            omf_free(buf);
        } else {
            a_dst->str_create();
        }
    }
    char* read_variable_str() {
        uint16_t len = read_uword();
        if (len == 0) return nullptr;
        char* str = static_cast<char*>(omf_calloc(len, 1));
        read_buf(str, len);
        return str;
    }

private:
    FILE* m_handle;
    long m_filesize;
    int m_sd_errno;
};

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_READER_HPP
