#ifndef OPENOMF_UTILS_STR_HPP
#define OPENOMF_UTILS_STR_HPP

#include "Vector.hpp"
#include <string>
#include <cstddef>
#include <cstdarg>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

namespace openomf {
namespace utils {

/** C utils/str 대응: 동적 문자열 (std::string 래핑). */
class Str {
public:
    Str() = default;
    ~Str() = default;
    Str(const Str&) = default;
    Str& operator=(const Str&) = default;

    void str_create();
    void str_from(const Str& a_src);
    void str_from_buf(const char* a_buf, size_t a_len);
    void str_from_c(const char* a_src);
    bool str_from_file(const char* a_file_name);
    void str_from_format(const char* a_format, ...);
    void str_from_slice(const Str& a_src, size_t a_start, size_t a_end);
    void str_format(const char* a_format, ...);
    void str_cut(size_t a_len);
    void str_cut_left(size_t a_len);
    void str_truncate(size_t a_max_len);
    void str_free();

    void str_toupper();
    void str_tolower();
    void str_rstrip();
    void str_lstrip();
    void str_strip();
    void str_append(const Str& a_src);
    void str_append_buf(const char* a_buf, size_t a_len);
    void str_append_c(const char* a_src);
    void str_append_char(char a_c);
    void str_append_format(const char* a_format, ...);
    void str_replace(const char* a_seek, const char* a_replacement, int a_limit);

    size_t str_size() const;
    bool str_find_next(char a_find, size_t* a_pos) const;
    bool str_first_of(char a_find, size_t* a_pos) const;
    bool str_last_of(char a_find, size_t* a_pos) const;
    bool str_equal(const Str& a_b) const;
    bool str_equal_buf(const char* a_buf, size_t a_len) const;
    bool str_equal_c(const char* a_b) const;
    char str_at(size_t a_pos) const;
    bool str_delete_at(size_t a_pos);
    bool str_set_at(size_t a_pos, char a_value);
    void str_set_c(const char* a_value);
    void str_set(const Str& a_value);
    bool str_insert_at(size_t a_pos, char a_value);
    bool str_insert_buf_at(size_t a_pos, const char* a_buf, size_t a_buf_len);
    bool str_insert_c_at(size_t a_pos, const char* a_src);

    bool str_to_float(float* a_out) const;
    bool str_to_long(long* a_out) const;
    bool str_to_int(int* a_out) const;

    const char* str_c() const;

    void str_split(Vector* a_dst, char a_ch) const;
    void str_split_c(Vector* a_dst, const char* a_src, char a_ch) const;

private:
    std::string m_data;
};

namespace detail {

inline void str_vector_free_cb(void* a_p)
{
    reinterpret_cast<Str*>(a_p)->~Str();
}

}  // namespace detail

inline void Str::str_create()
{
    m_data.clear();
}

inline void Str::str_from(const Str& a_src)
{
    m_data = a_src.m_data;
}

inline void Str::str_from_buf(const char* a_buf, size_t a_len)
{
    m_data.assign(a_buf, a_len);
}

inline void Str::str_from_c(const char* a_src)
{
    m_data = a_src ? a_src : "";
}

inline bool Str::str_from_file(const char* a_file_name)
{
    std::ifstream f(a_file_name, std::ios::binary);
    if (!f) {
        return false;
    }
    std::ostringstream os;
    os << f.rdbuf();
    m_data = os.str();
    return true;
}

inline void Str::str_from_format(const char* a_format, ...)
{
    va_list args;
    va_start(args, a_format);
    char buf[4096];
    int n = std::vsnprintf(buf, sizeof(buf), a_format, args);
    va_end(args);
    if (n >= 0 && n < static_cast<int>(sizeof(buf))) {
        m_data.assign(buf, static_cast<size_t>(n));
    } else if (n > 0) {
        std::vector<char> large(static_cast<size_t>(n) + 1);
        va_start(args, a_format);
        std::vsnprintf(large.data(), large.size(), a_format, args);
        va_end(args);
        m_data.assign(large.data());
    }
}

inline void Str::str_from_slice(const Str& a_src, size_t a_start, size_t a_end)
{
    size_t len = a_src.str_size();
    if (a_start > len) {
        a_start = len;
    }
    if (a_end > len) {
        a_end = len;
    }
    if (a_start >= a_end) {
        m_data.clear();
        return;
    }
    m_data = a_src.m_data.substr(a_start, a_end - a_start);
}

inline void Str::str_format(const char* a_format, ...)
{
    va_list args;
    va_start(args, a_format);
    char buf[4096];
    int n = std::vsnprintf(buf, sizeof(buf), a_format, args);
    va_end(args);
    if (n >= 0 && n < static_cast<int>(sizeof(buf))) {
        m_data.assign(buf, static_cast<size_t>(n));
    } else if (n > 0) {
        std::vector<char> large(static_cast<size_t>(n) + 1);
        va_start(args, a_format);
        std::vsnprintf(large.data(), large.size(), a_format, args);
        va_end(args);
        m_data.assign(large.data());
    }
}

inline void Str::str_cut(size_t a_len)
{
    if (a_len >= m_data.size()) {
        m_data.clear();
    } else {
        m_data.resize(m_data.size() - a_len);
    }
}

inline void Str::str_cut_left(size_t a_len)
{
    if (a_len >= m_data.size()) {
        m_data.clear();
    } else {
        m_data.erase(0, a_len);
    }
}

inline void Str::str_truncate(size_t a_max_len)
{
    if (m_data.size() > a_max_len) {
        m_data.resize(a_max_len);
    }
}

inline void Str::str_free()
{
    m_data.clear();
}

inline void Str::str_toupper()
{
    for (auto& c : m_data) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
}

inline void Str::str_tolower()
{
    for (auto& c : m_data) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
}

inline void Str::str_rstrip()
{
    size_t i = m_data.size();
    while (i > 0 && std::isspace(static_cast<unsigned char>(m_data[i - 1]))) {
        --i;
    }
    m_data.resize(i);
}

inline void Str::str_lstrip()
{
    size_t i = 0;
    while (i < m_data.size() && std::isspace(static_cast<unsigned char>(m_data[i]))) {
        ++i;
    }
    m_data.erase(0, i);
}

inline void Str::str_strip()
{
    str_lstrip();
    str_rstrip();
}

inline void Str::str_append(const Str& a_src)
{
    m_data += a_src.m_data;
}

inline void Str::str_append_buf(const char* a_buf, size_t a_len)
{
    m_data.append(a_buf, a_len);
}

inline void Str::str_append_c(const char* a_src)
{
    m_data += (a_src ? a_src : "");
}

inline void Str::str_append_char(char a_c)
{
    m_data += a_c;
}

inline void Str::str_append_format(const char* a_format, ...)
{
    va_list args;
    va_start(args, a_format);
    char buf[4096];
    int n = std::vsnprintf(buf, sizeof(buf), a_format, args);
    va_end(args);
    if (n >= 0 && n < static_cast<int>(sizeof(buf))) {
        m_data.append(buf, static_cast<size_t>(n));
    }
}

inline void Str::str_replace(const char* a_seek, const char* a_replacement, int a_limit)
{
    if (!a_seek || !*a_seek) {
        return;
    }
    std::string seek(a_seek);
    std::string repl(a_replacement ? a_replacement : "");
    size_t pos = 0;
    int count = 0;
    while ((pos = m_data.find(seek, pos)) != std::string::npos) {
        m_data.replace(pos, seek.size(), repl);
        pos += repl.size();
        if (a_limit >= 0 && ++count >= a_limit) {
            break;
        }
    }
}

inline size_t Str::str_size() const
{
    return m_data.size();
}

inline bool Str::str_find_next(char a_find, size_t* a_pos) const
{
    if (!a_pos || *a_pos >= m_data.size()) {
        return false;
    }
    size_t p = m_data.find(a_find, *a_pos);
    if (p == std::string::npos) {
        return false;
    }
    *a_pos = p;
    return true;
}

inline bool Str::str_first_of(char a_find, size_t* a_pos) const
{
    if (!a_pos) {
        return false;
    }
    size_t p = m_data.find(a_find);
    if (p == std::string::npos) {
        return false;
    }
    *a_pos = p;
    return true;
}

inline bool Str::str_last_of(char a_find, size_t* a_pos) const
{
    if (!a_pos) {
        return false;
    }
    size_t p = m_data.rfind(a_find);
    if (p == std::string::npos) {
        return false;
    }
    *a_pos = p;
    return true;
}

inline bool Str::str_equal(const Str& a_b) const
{
    return m_data == a_b.m_data;
}

inline bool Str::str_equal_buf(const char* a_buf, size_t a_len) const
{
    return m_data.size() == a_len &&
           (a_len == 0 || std::memcmp(m_data.data(), a_buf, a_len) == 0);
}

inline bool Str::str_equal_c(const char* a_b) const
{
    return a_b && m_data == a_b;
}

inline char Str::str_at(size_t a_pos) const
{
    return (a_pos < m_data.size()) ? m_data[a_pos] : '\0';
}

inline bool Str::str_delete_at(size_t a_pos)
{
    if (a_pos >= m_data.size()) {
        return false;
    }
    m_data.erase(a_pos, 1);
    return true;
}

inline bool Str::str_set_at(size_t a_pos, char a_value)
{
    if (a_pos >= m_data.size()) {
        return false;
    }
    m_data[a_pos] = a_value;
    return true;
}

inline void Str::str_set_c(const char* a_value)
{
    m_data = a_value ? a_value : "";
}

inline void Str::str_set(const Str& a_value)
{
    m_data = a_value.m_data;
}

inline bool Str::str_insert_at(size_t a_pos, char a_value)
{
    if (a_pos > m_data.size()) {
        return false;
    }
    m_data.insert(a_pos, 1, a_value);
    return true;
}

inline bool Str::str_insert_buf_at(size_t a_pos, const char* a_buf, size_t a_buf_len)
{
    if (a_pos > m_data.size()) {
        return false;
    }
    m_data.insert(a_pos, a_buf, a_buf_len);
    return true;
}

inline bool Str::str_insert_c_at(size_t a_pos, const char* a_src)
{
    if (!a_src || a_pos > m_data.size()) {
        return false;
    }
    m_data.insert(a_pos, a_src);
    return true;
}

inline bool Str::str_to_float(float* a_out) const
{
    if (!a_out || m_data.empty()) {
        return false;
    }
    try {
        *a_out = std::stof(m_data);
        return true;
    } catch (...) {
        return false;
    }
}

inline bool Str::str_to_long(long* a_out) const
{
    if (!a_out || m_data.empty()) {
        return false;
    }
    try {
        *a_out = std::stol(m_data);
        return true;
    } catch (...) {
        return false;
    }
}

inline bool Str::str_to_int(int* a_out) const
{
    if (!a_out || m_data.empty()) {
        return false;
    }
    try {
        *a_out = std::stoi(m_data);
        return true;
    } catch (...) {
        return false;
    }
}

inline const char* Str::str_c() const
{
    return m_data.c_str();
}

inline void Str::str_split(Vector* a_dst, char a_ch) const
{
    if (!a_dst) {
        return;
    }
    a_dst->vector_create_with_size_cb(sizeof(Str), 0, detail::str_vector_free_cb);
    size_t start = 0;
    for (size_t i = 0; i <= m_data.size(); ++i) {
        if (i == m_data.size() || m_data[i] == a_ch) {
            void* slot = a_dst->vector_append_ptr();
            Str* s = new (slot) Str();
            s->str_from_buf(m_data.data() + start, i - start);
            start = i + 1;
        }
    }
}

inline void Str::str_split_c(Vector* a_dst, const char* a_src, char a_ch) const
{
    if (!a_dst || !a_src) {
        return;
    }
    a_dst->vector_create_with_size_cb(sizeof(Str), 0, detail::str_vector_free_cb);
    const char* start = a_src;
    for (const char* p = a_src;; ++p) {
        if (*p == a_ch || *p == '\0') {
            void* slot = a_dst->vector_append_ptr();
            Str* s = new (slot) Str();
            s->str_from_buf(start, static_cast<size_t>(p - start));
            if (*p == '\0') {
                break;
            }
            start = p + 1;
        }
    }
}

}  // namespace utils
}  // namespace openomf

#endif
