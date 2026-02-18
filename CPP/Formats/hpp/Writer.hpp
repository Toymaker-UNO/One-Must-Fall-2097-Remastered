#ifndef OPENOMF_FORMATS_WRITER_HPP
#define OPENOMF_FORMATS_WRITER_HPP

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

/** C formats/internal/writer (sd_writer) 대응. 바이너리 파일 쓰기. */
class Writer {
public:
    Writer() : m_handle(nullptr), m_sd_errno(0) {}

    static Writer* open(const char* a_file) {
        FILE* handle = openomf::utils::Io::file_open(a_file, "wb");
        if (!handle) return nullptr;
        Writer* w = static_cast<Writer*>(omf_calloc(1, sizeof(Writer)));
        w->m_handle = handle;
        w->m_sd_errno = 0;
        return w;
    }

    void close() {
        if (m_handle) openomf::utils::Io::file_close(m_handle);
        m_handle = nullptr;
        openomf::utils::Allocator::free_real(this);
    }

    int errno_val() const { return m_sd_errno; }
    long pos() const {
        long res = ftell(m_handle);
        if (res == -1) const_cast<Writer*>(this)->m_sd_errno = errno;
        return res;
    }
    int seek_start(long a_offset) { return fseek(m_handle, a_offset, SEEK_SET); }
    int seek_cur(long a_offset) { return fseek(m_handle, a_offset, SEEK_CUR); }
    int seek_end(long a_offset) { return fseek(m_handle, a_offset, SEEK_END); }

    int write_buf(const char* a_buf, size_t a_len) {
        if (a_buf == nullptr) {
            if (a_len == 0) return 1;
            m_sd_errno = EINVAL;
            return 0;
        }
        if (fwrite(a_buf, 1, a_len, m_handle) != a_len) {
            m_sd_errno = ferror(m_handle);
            return 0;
        }
        return 1;
    }
    int write_fprintf(const char* a_format, ...) {
        std::va_list argp;
        va_start(argp, a_format);
        int ret = vfprintf(m_handle, a_format, argp);
        va_end(argp);
        return ret;
    }
    void write_ubyte(uint8_t a_data) { write_buf(reinterpret_cast<const char*>(&a_data), 1); }
    void write_uword(uint16_t a_data) { write_buf(reinterpret_cast<const char*>(&a_data), 2); }
    void write_udword(uint32_t a_data) { write_buf(reinterpret_cast<const char*>(&a_data), 4); }
    void write_byte(int8_t a_data) { write_buf(reinterpret_cast<const char*>(&a_data), 1); }
    void write_word(int16_t a_data) { write_buf(reinterpret_cast<const char*>(&a_data), 2); }
    void write_dword(int32_t a_data) { write_buf(reinterpret_cast<const char*>(&a_data), 4); }
    void write_float(float a_data) { write_buf(reinterpret_cast<const char*>(&a_data), sizeof(float)); }
    void write_fill(char a_content, size_t a_len) {
        char buffer[1024];
        std::memset(buffer, a_content, 1024);
        size_t left = a_len;
        while (left > 0) {
            size_t n = (left > 1024) ? 1024 : left;
            if (fwrite(buffer, 1, n, m_handle) != n) {
                m_sd_errno = ferror(m_handle);
                return;
            }
            left -= n;
        }
    }
    void write_str(const openomf::utils::Str* a_src, bool a_null_terminated) {
        if (a_src->str_size() == 0) {
            write_uword(0);
            return;
        }
        if (a_null_terminated) {
            write_uword(static_cast<uint16_t>(a_src->str_size() + 1));
            write_buf(a_src->str_c(), a_src->str_size());
            write_ubyte(0);
        } else {
            write_uword(static_cast<uint16_t>(a_src->str_size()));
            write_buf(a_src->str_c(), a_src->str_size());
        }
    }
    void write_variable_str(const char* a_str) {
        if (a_str == nullptr) { write_uword(0); return; }
        uint16_t len = static_cast<uint16_t>(std::strlen(a_str) + 1);
        write_uword(len);
        write_buf(a_str, len);
    }

private:
    FILE* m_handle;
    int m_sd_errno;
};

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_WRITER_HPP
