#ifndef OPENOMF_UTILS_ALLOCATOR_HPP
#define OPENOMF_UTILS_ALLOCATOR_HPP

#include <cstdlib>
#include <cstdio>
#include <cassert>

namespace openomf {
namespace utils {

/** C utils/allocator 대응: 실패 시 로그 후 abort. */
class Allocator {
public:
    /** size 바이트 할당. 0은 허용하지 않음. */
    static void* malloc_real(size_t a_size, const char* a_file, int a_line) {
        assert(a_size > 0);
        void* ret = std::malloc(a_size);
        if (ret != nullptr) return ret;
        std::fprintf(stderr, "%s", text_malloc_error());
        std::fprintf(stderr, "malloc(%zu) failed on %s:%d\n", a_size, a_file, a_line);
        std::abort();
    }

    /** nmemb * size 바이트를 0으로 초기화하여 할당. */
    static void* calloc_real(size_t a_nmemb, size_t a_size, const char* a_file, int a_line) {
        assert(a_size > 0);
        assert(a_nmemb > 0);
        void* ret = std::calloc(a_nmemb, a_size);
        if (ret != nullptr) return ret;
        std::fprintf(stderr, "calloc(%zu, %zu) failed on %s:%d\n", a_nmemb, a_size, a_file, a_line);
        std::abort();
    }

    /** 기존 ptr을 a_size 크기로 리사이즈. */
    static void* realloc_real(void* a_ptr, size_t a_size, const char* a_file, int a_line) {
        assert(a_size > 0);
        void* ret = std::realloc(a_ptr, a_size);
        if (ret != nullptr) return ret;
        std::fprintf(stderr, "realloc(%p, %zu) failed on %s:%d\n", a_ptr, a_size, a_file, a_line);
        std::abort();
    }

    /** 할당 해제 (ptr은 호출 후 사용하지 않음; C 매크로처럼 NULL 대입은 호출부에서). */
    static void free_real(void* a_ptr) { std::free(a_ptr); }

    static const char* text_malloc_error() { return "malloc error"; }
    static const char* text_calloc_error() { return "calloc error"; }
    static const char* text_realloc_error() { return "realloc error"; }
};

}  // namespace utils
}  // namespace openomf

#define omf_malloc(size) \
    openomf::utils::Allocator::malloc_real((size), __FILE__, __LINE__)
#define omf_calloc(nmemb, size) \
    openomf::utils::Allocator::calloc_real((nmemb), (size), __FILE__, __LINE__)
#define omf_realloc(ptr, size) \
    openomf::utils::Allocator::realloc_real((ptr), (size), __FILE__, __LINE__)
#define omf_free(ptr) \
    do { openomf::utils::Allocator::free_real(ptr); (ptr) = nullptr; } while (0)

#endif
